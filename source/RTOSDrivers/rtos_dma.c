#include "rtos_dma.h"
#include "Drivers/dma.h"
#include "RTOS/rtos.h"

void rtos_dma_wait_until_end_callback() {
	// Ensure correct DMAC channel is being checked
	DMAC_REGS->DMAC_CHID = *((uint8_t*)(current_process->data));

	// // for evaluation
	// if (DMAC_REGS->DMAC_CHINTFLAG & DMAC_CHINTFLAG_TCMPL(1))
	// 	current_process->status = Process_State_Running;
}

void rtos_dma_wait_until_end(int channel) {
	// save value of current_process->data as it is changed by this function
	void* data = current_process->data;
	current_process->data = &channel;
	wait_until_callback(&(DMAC_REGS->DMAC_CHINTFLAG), DMAC_CHINTFLAG_TCMPL(1), DMAC_CHINTFLAG_TCMPL(1),
		Process_Wait_Until_Equal, rtos_dma_wait_until_end_callback);
	// wait_until_callback(&(DMAC_REGS->DMAC_CHINTFLAG), DMAC_CHINTFLAG_TCMPL(1), DMAC_CHINTFLAG_TCMPL(1),
	// 	Process_Wait_Until_None, rtos_dma_wait_until_end_callback);
	// while (!(DMAC_REGS->DMAC_CHINTFLAG & DMAC_CHINTFLAG_TCMPL(1))) DMAC_REGS->DMAC_CHID = channel;
	current_process->data = data;
}

void rtos_dma_wait_until_end_or_timeout_callback() {
	// Ensure correct DMAC channel is being checked
	DMAC_REGS->DMAC_CHID = *((uint8_t*)(current_process->data));

	// Check whether timeout has occured
	volatile uint32_t time = time_read_ticks();
	int t = (int)(current_process->return_deadline - time);
	static volatile int min = INT32_MAX;
	min = (t < min) ? t : min;
	if (t <= 0) current_process->status = Process_State_Running;
}

void rtos_dma_wait_until_end_or_timeout(int channel, uint32_t timeout_ticks) {
	// set return deadline for process
	current_process->return_deadline = time_read_ticks() + timeout_ticks;

	// save value of current_process->data as it is changed by this function
	void* data = current_process->data;
	current_process->data = &channel;
	wait_until_callback_preserve_deadline(&(DMAC_REGS->DMAC_CHINTFLAG), DMAC_CHINTFLAG_TCMPL(1), DMAC_CHINTFLAG_TCMPL(1),
		Process_Wait_Until_Equal, rtos_dma_wait_until_end_or_timeout_callback);
		
	current_process->data = data;
}