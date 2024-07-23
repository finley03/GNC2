#ifndef COMMS_H
#define COMMS_H

#include <stdbool.h>

// Test communications by introducing random errors
// #define COMMS_TEST

#ifdef COMMS_TEST
#include <stdlib.h>
#endif

bool init_comms();

bool comms_loop();

#endif