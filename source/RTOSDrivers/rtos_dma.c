#include "rtos_dma.h"
#include "Drivers/dma.h"
#include "RTOS/rtos.h"

void rtos_dma_wait_until_end_callback() {
	DMAC_REGS->DMAC_CHID = *((uint8_t*)(current_process->data));
}

void rtos_dma_wait_until_end(int channel) {
	// save value of current_process->data as it is changed by this function
	void* data = current_process->data;
	current_process->data = &channel;
	wait_until_callback(&(DMAC_REGS->DMAC_CHINTFLAG), DMAC_CHINTFLAG_TCMPL(1), DMAC_CHINTFLAG_TCMPL(1),
		Process_Wait_Until_Equal, rtos_dma_wait_until_end_callback);
	current_process->data = data;
}