#include "navigation.h"
#include "RTOS/rtos.h"
#include "global_imu.h"
#include "util.h"

Process navigation_process;

void navigation() {
	while (1) {
		if (!globals.RunNavProcess) {
			wait_until_true(&globals.RunNavProcess);
		}
        // led_on();
		request_imu_data();
		wait_for_imu_data();
        // led_off();
		rtos_delay_ms(5);
	}
}

void start_navigation() {
    init_process(&navigation_process, navigation, NAVIGATION_STACK_BASE, NAVIGATION_STACK_SIZE);
    dispatch_process(&navigation_process);
}