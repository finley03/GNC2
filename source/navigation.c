#include "navigation.h"
#include "RTOS/rtos.h"
#include "global_imu.h"
#include "util.h"

Process navigation_process;

void navigation() {
	while (1) {
        // led_on();
		request_imu_data();
		wait_for_imu_data();
        // led_off();
		rtos_delay_ms(1);
	}
}

void start_navigation() {
    init_process(&navigation_process, navigation, NAVIGATION_STACK_BASE, NAVIGATION_STACK_SIZE);
    dispatch_process(&navigation_process);
}