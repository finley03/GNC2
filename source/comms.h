#ifndef COMMS_H
#define COMMS_H

#include <stdbool.h>

// Test communications by introducing random errors
// #define COMMS_TEST

#ifdef COMMS_TEST
#include <stdlib.h>
#endif

#include "global_ports.h"

bool init_comms(Serial_Port_IDs port);

bool comms_loop();

#endif