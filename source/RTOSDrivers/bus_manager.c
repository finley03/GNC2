#include "bus_manager.h"
#include TIME_DRIVER

// set all requests with device IDs matching the given request to the given status
void set_matching_id_status(BusManager_Process_Data* data, BusManager_Transaction_Request* request, int queue_pointer, int8_t status) {
	int device_id = request->device_id;
	
	BusManager_Transaction_Request* current_request;
	
	for (int count = 1; count < data->request_count; ++count) {
		// increment
		++queue_pointer;
		queue_pointer %= BUSMGR_PROCESS_REQUEST_QUEUE_LENGTH;
		
		// set current request
		current_request = data->request_queue[queue_pointer];
		
		// if match set status
		if (current_request->device_id == device_id) current_request->status = status;
	}
}

void busmanager_process_wait_callback() {
	// check if busy processes are no longer busy
	// if one is found, change process status to running
	BusManager_Process_Data* data = (BusManager_Process_Data*)(current_process->data);
	BusManager_Transaction_Request* current_request;
	int queue_pointer = data->request_queue_head;
	
	for (int i = 0; i < data->request_count; ++i) {
		current_request = data->request_queue[queue_pointer];
		// if busy
		if (current_request->busy) {
			int v = (int)(current_request->busy_until - time_read_ticks());
			if (v <= 0)
				current_process->status = Process_State_Running;
			//if ((int)(current_request->busy_until - time_read_ticks()) <= 0) current_process->status = Process_State_Running;
		}
		
		++queue_pointer;
		queue_pointer %= BUSMGR_PROCESS_REQUEST_QUEUE_LENGTH;
	}
}

void busmanager_process_function() {
	// create typed pointer to process data
	BusManager_Process_Data* data = (BusManager_Process_Data*)(current_process->data);
	
	// current request being processed
	BusManager_Transaction_Request* current_request;
	
	// message loop
	while (1) {
		// wait until number of requests is greater than 0
		if (data->request_count == 0) wait_until(&(data->request_count), 0, INT_MASK, Process_Wait_Until_Greater);
		
		// find first unblocked process, or unblock if possible
		int queue_pointer = data->request_queue_head;
		int blocked_count = 0;
		
		for (; blocked_count < data->request_count; ++blocked_count) {
			bool unblocked = false;
			current_request = data->request_queue[queue_pointer];
			//// break if ready and not blocked
			if (current_request->status == BusManager_Request_Ready && !current_request->busy) break;
			// if busy attempt to set busy state to false
			if (current_request->busy) {
				// check if busy time has passed
				if ((int)(current_request->busy_until - time_read_ticks()) <= 0) {
					// set busy to false
					current_request->busy = false;
					// set requests for the same id to unblocked
					set_matching_id_status(data, current_request, queue_pointer, BusManager_Request_Ready);
					// set unblocked
					unblocked = true;
				}
				else {
					set_matching_id_status(data, current_request, queue_pointer, BusManager_Request_Blocked);
				}
			}
			// check if unblocked
			if (unblocked) break;
			
			// increment queue pointer
			++queue_pointer;
			queue_pointer %= BUSMGR_PROCESS_REQUEST_QUEUE_LENGTH;
		}
		
		// check if all is blocked
		// if so, hand control to OS until either a request is unblocked
		// or a new request is added
		if (blocked_count == data->request_count) {
			wait_until_callback(&(data->request_count), data->request_count, INT_MASK,
			Process_Wait_Until_NotEqual, busmanager_process_wait_callback);
			continue;
		}
		
		// execute next request
		if (current_request->status != BusManager_Request_Complete) {
			// data->execute_function(current_request, data->port_descriptor);
			if (!current_request->request_processor_function)
				SOS();
			if (current_request->request_processor_function(current_request->device_descriptor, data->port_descriptor, current_request->request_data, current_request)) {
				current_request->status = BusManager_Request_Complete;

				// if (!current_request->busy) {
				// // move blocked processes up queue
				// 	for (; blocked_count + 1 > 0; --blocked_count) {
				// 		int source_pointer = (data->request_queue_head + blocked_count - 1) % RTOS_MAX_PROCESS_COUNT;
				// 		int destination_pointer = (data->request_queue_head + blocked_count) % RTOS_MAX_PROCESS_COUNT;
				// 		data->request_queue[destination_pointer] = data->request_queue[source_pointer];
				// 	}
				// }
			}
		}
		
		// clear request if done and device is not busy
		// done request acts as a record of if the device is busy, and which request
		// it is busy executing
		if (current_request->status == BusManager_Request_Complete && !current_request->busy) {
			current_request->status = BusManager_Request_CompleteNotBusy;

			// move blocked processes up queue
			for (; blocked_count > 0; --blocked_count) {
				int source_pointer = (data->request_queue_head + blocked_count - 1) % BUSMGR_PROCESS_REQUEST_QUEUE_LENGTH;
				int destination_pointer = (data->request_queue_head + blocked_count) % BUSMGR_PROCESS_REQUEST_QUEUE_LENGTH;
				data->request_queue[destination_pointer] = data->request_queue[source_pointer];
			}

			--(data->request_count);
			++(data->request_queue_head);
			data->request_queue_head %= BUSMGR_PROCESS_REQUEST_QUEUE_LENGTH;
		}
	}
}

// void busmanager_new_process(BusManager_Process* proc, uint32_t stack_base, uint32_t stack_size, sercom_registers_t* sercom,
// void* port_descriptor, void (*execute_function)(BusManager_Transaction_Request*, void*)) {
// 	init_process(&(proc->process), busmanager_process_function, stack_base, stack_size);
// 	proc->data.sercom = sercom;
// 	proc->data.port_descriptor = port_descriptor;
// 	proc->data.execute_function = execute_function;
// 	proc->data.request_queue_head = 0;
// 	proc->data.request_queue_tail = 0;
// 	proc->data.request_count = 0;
// 	proc->process.data = &(proc->data);
// 	dispatch_process(&(proc->process));
// }

void busmanager_new_process(BusManager_Process* proc, uint32_t stack_base, uint32_t stack_size, sercom_registers_t* sercom,
void* port_descriptor) {
	init_process(&(proc->process), busmanager_process_function, stack_base, stack_size);
	proc->data.sercom = sercom;
	proc->data.port_descriptor = port_descriptor;
	proc->data.request_queue_head = 0;
	proc->data.request_queue_tail = 0;
	proc->data.request_count = 0;
	proc->process.data = &(proc->data);
	dispatch_process(&(proc->process));
}

// void busmanager_request_transaction(BusManager_Process* proc, BusManager_Transaction_Request* request,
// int device_id, void* device_descriptor, void* request_data) {
// 	// set request info
// 	request->device_id = device_id;
// 	request->device_descriptor = device_descriptor;
// 	request->request_data = request_data;
// 	request->busy = false;
// 	//request->blocked = false;
// 	//request->complete = false;
// 	request->status = BusManager_Request_Ready;
	
// 	// push to request queue
// 	// if queue is full wait until there is space
// 	// in while loop to avoid race condition if two processes are pushing a request at once
// 	while (proc->data.request_count == BUSMGR_PROCESS_REQUEST_QUEUE_LENGTH)
// 		wait_until(&(proc->data.request_count), BUSMGR_PROCESS_REQUEST_QUEUE_LENGTH, INT_MASK, Process_Wait_Until_NotEqual);
	
// 	// now there is guaranteed to be space in the queue
// 	// add request at tail
// 	proc->data.request_queue[proc->data.request_queue_tail] = request;
// 	// increment request count
// 	++(proc->data.request_count);
// 	// increment tail and overflow
// 	++(proc->data.request_queue_tail);
// 	proc->data.request_queue_tail %= BUSMGR_PROCESS_REQUEST_QUEUE_LENGTH;
// }

void busmanager_request_transaction(BusManager_Process* proc, BusManager_Transaction_Request* request,
	int device_id, void* device_descriptor, void* request_data,
	bool (*request_processor_function)(void* device_descriptor, void* port_descriptor, void* request, BusManager_Transaction_Request* busrequest)) {
	// set request info
	request->device_id = device_id;
	request->device_descriptor = device_descriptor;
	request->request_data = request_data;
	request->request_processor_function = request_processor_function;
	request->busy = false;
	//request->blocked = false;
	//request->complete = false;
	request->status = BusManager_Request_Ready;
	
	// push to request queue
	// if queue is full wait until there is space
	// in while loop to avoid race condition if two processes are pushing a request at once
	while (proc->data.request_count == BUSMGR_PROCESS_REQUEST_QUEUE_LENGTH)
		wait_until(&(proc->data.request_count), BUSMGR_PROCESS_REQUEST_QUEUE_LENGTH, INT_MASK, Process_Wait_Until_NotEqual);
	
	// now there is guaranteed to be space in the queue
	// add request at tail
	proc->data.request_queue[proc->data.request_queue_tail] = request;
	// increment request count
	++(proc->data.request_count);
	// increment tail and overflow
	++(proc->data.request_queue_tail);
	proc->data.request_queue_tail %= BUSMGR_PROCESS_REQUEST_QUEUE_LENGTH;
}