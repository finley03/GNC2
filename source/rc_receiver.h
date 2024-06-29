#ifndef RC_RECEIVER_H
#define RC_RECEIVER_H

// Read serial signals from an RC receiver
// currently implemented protocol is crossfire (CRSF)

#include "global_ports.h"

bool receiver_init(Serial_Port_IDs port);

#endif