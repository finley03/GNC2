#ifndef BUS_MANAGER_H
#define BUS_MANAGER_H

#include <stdbool.h>
#include <sam.h>
#include "RTOS/rtos.h"

#define BUSMGR_PROCESS_REQUEST_QUEUE_LENGTH 8

typedef enum {
	BusManager_Request_Ready, // ready to be executed, no transfers have been executed
	BusManager_Request_Blocked, // unable to execute at this time, device is busy
	BusManager_Request_Complete, // all transfers have finished, but device may still be busy
	BusManager_Request_CompleteNotBusy // all transfers have finished and device is no longer busy
} BusManager_Request_Status;

typedef struct __attribute__((packed)) {
	// device identifier
	int device_id;
	// descriptor for device
	void* device_descriptor;
	// data of request
	void* request_data;
	// function to process the request
	bool (*request_processor_function)(void* device_descriptor, void* port_descriptor, void* request, void* busrequest);
	// time device is busy until
	uint32_t busy_until;
	// device busy executing request
	bool busy;
	// request status
	int8_t status;
} BusManager_Transaction_Request;

typedef struct {
	// sercom process is responsible for
	sercom_registers_t* sercom;
	// port descriptor
	void* port_descriptor;
	// // execute request function
	// void (*execute_function)(BusManager_Transaction_Request*, void*);
	// request queue
	BusManager_Transaction_Request* request_queue[BUSMGR_PROCESS_REQUEST_QUEUE_LENGTH];
	// queue head
	int request_queue_head;
	// queue tail
	int request_queue_tail;
	// number of requests in queue
	int request_count;
} BusManager_Process_Data;

typedef struct {
	Process process;
	BusManager_Process_Data data;
} BusManager_Process;

// create new bus manager process
// execute_function arguments must be in the form of (request, port_descriptor)
// void busmanager_new_process(BusManager_Process* proc, uint32_t stack_base, uint32_t stack_size, sercom_registers_t* sercom,
// void* port_descriptor, void (*execute_function)(BusManager_Transaction_Request*, void*));
void busmanager_new_process(BusManager_Process* proc, uint32_t stack_base, uint32_t stack_size, sercom_registers_t* sercom,
void* port_descriptor);

void busmanager_request_transaction(BusManager_Process* proc, BusManager_Transaction_Request* request,
	int device_id, void* device_descriptor, void* request_data, bool (*request_processor_function)(void* device_descriptor,
	void* port_descriptor, void* request, BusManager_Transaction_Request* busrequest));

#endif