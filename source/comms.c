#include "comms.h"
#include "gnclink.h"
#include "globals.h"
#include "RTOS/rtos.h"
#include PORT_DRIVER

#include <string.h>

static Serial_Port_IDs comms_port = __PORT_ID_END; // PORT4

uint8_t rxpacket[GNCLINK_PACKET_MAX_TOTAL_LENGTH];
uint8_t txpacket[GNCLINK_PACKET_MAX_TOTAL_LENGTH];
uint8_t rxframe[GNCLINK_FRAME_TOTAL_LENGTH];
uint8_t txframe[GNCLINK_FRAME_TOTAL_LENGTH];

uint8_t rxframe_buffer[GNCLINK_FRAME_TOTAL_LENGTH * 2];

extern void SOS();

bool init_comms(Serial_Port_IDs port) {
    comms_port = port;

    // Start infinite receive buffer only if not usb vcp
    if (!serial_is_usb_vcp(comms_port)) {
	    if (!serial_read_start_infinite(comms_port, rxframe_buffer, sizeof(rxframe_buffer))) return false;
    }

    return true;
}


bool getGlobalHash() {
    uint32_t* payload = (uint32_t*)GNClink_Get_Packet_Payload_Pointer(txpacket);
    *payload = __GLOBAL_HASH;
    if (!GNClink_Construct_Packet(txpacket, GNClink_PacketType_GetGlobalHash, GNClink_PacketFlags_Response, 4)) return false;
    return true;
}

bool getValueList() {
    volatile uint8_t* rxpayload = GNClink_Get_Packet_Payload_Pointer(rxpacket); // no idea why this needs to be volatile???
    uint8_t* txpayload = GNClink_Get_Packet_Payload_Pointer(txpacket);
    uint16_t* idList = (uint16_t*)(rxpayload + 1);

    // get number of IDs in list
    int IDCount = (int)*rxpayload;
    if (IDCount > (GNCLINK_PACKET_MAX_PAYLOAD_LENGTH - 1) / 2) {
        // too many IDs, do something...
        return false;
    }

    int payloadIndex = 0;
    for (int index = 0; index < IDCount; ++index) {
        Global_Variable_IDs ID = (Global_Variable_IDs)idList[index];
        void* address;
        int size;
        if (!get_global_address_size(ID, &address, &size)) {
            // invalid ID, do something...
            return false;
        }

        if (payloadIndex + size > GNCLINK_PACKET_MAX_PAYLOAD_LENGTH) {
            // no more space, do something...
            return false;
        }

        // copy value to payload
        memcpy(&txpayload[payloadIndex], address, size);
        payloadIndex += size;
    }

    // construct response packet
    if (!GNClink_Construct_Packet(txpacket, GNClink_PacketType_GetValueList, GNClink_PacketFlags_Response, payloadIndex)) return false;
    return true;
}

bool setValueList() {
    volatile uint8_t* rxpayload = GNClink_Get_Packet_Payload_Pointer(rxpacket); // no idea why this needs to be volatile???
    uint8_t* txpayload = GNClink_Get_Packet_Payload_Pointer(txpacket);

    // get number of IDs in list
    int IDCount = (int)*rxpayload;

    int payloadIndex = 1;
    for (int index = 0; index < IDCount; ++index) {
        Global_Variable_IDs ID = (Global_Variable_IDs)*((uint16_t*)(rxpayload + payloadIndex));
        void* address;
        int size;
        if (!get_global_address_size(ID, &address, &size)) {
            // invalid ID, do something...
            return false;
        }

        if (payloadIndex + size + 2 > GNCLINK_PACKET_MAX_PAYLOAD_LENGTH) {
            // Buffer overrun, do something...
            return false;
        }

        // copy value from payload to globals
        memcpy(address, &rxpayload[payloadIndex + 2], size);
        payloadIndex += size + 2;
    }

    // construct empty response packet
    if (!GNClink_Construct_Packet(txpacket, GNClink_PacketType_SetValueList, GNClink_PacketFlags_Response, 0)) return false;
    return true;
}

bool loadValueList() {
    volatile uint8_t* rxpayload = GNClink_Get_Packet_Payload_Pointer(rxpacket); // no idea why this needs to be volatile???
    uint8_t* txpayload = GNClink_Get_Packet_Payload_Pointer(txpacket);
    uint16_t* idList = (uint16_t*)(rxpayload + 1);

    // get number of IDs in list
    int IDCount = (int)*rxpayload;

    for (int index = 0; index < IDCount; ++index) {
        Global_Variable_IDs ID = (Global_Variable_IDs)idList[index];

        if (!load_global(ID)) {
            // Invalid ID
            return false;
        }
    }

    // construct empty response packet
    if (!GNClink_Construct_Packet(txpacket, GNClink_PacketType_LoadValueList, GNClink_PacketFlags_Response, 0)) return false;
    return true;
}

bool saveValueList() {
    volatile uint8_t* rxpayload = GNClink_Get_Packet_Payload_Pointer(rxpacket); // no idea why this needs to be volatile???
    uint8_t* txpayload = GNClink_Get_Packet_Payload_Pointer(txpacket);
    uint16_t* idList = (uint16_t*)(rxpayload + 1);

    // get number of IDs in list
    int IDCount = (int)*rxpayload;

    for (int index = 0; index < IDCount; ++index) {
        Global_Variable_IDs ID = (Global_Variable_IDs)idList[index];

        if (!save_global(ID)) {
            // Invalid ID
            return false;
        }
    }

    // construct empty response packet
    if (!GNClink_Construct_Packet(txpacket, GNClink_PacketType_SaveValueList, GNClink_PacketFlags_Response, 0)) return false;
    return true;
}

bool getValueCount() {
    uint16_t* payload = (uint32_t*)GNClink_Get_Packet_Payload_Pointer(txpacket);
    *payload = __GLOBAL_ID_END - __GLOBAL_ID_START;
    if (!GNClink_Construct_Packet(txpacket, GNClink_PacketType_GetValueCount, GNClink_PacketFlags_Response, 2)) return false;
    return true;
}

bool getValueName() {
    uint16_t* rxpayload = (uint16_t*)GNClink_Get_Packet_Payload_Pointer(rxpacket);
    uint8_t* txpayload = GNClink_Get_Packet_Payload_Pointer(txpacket);

    // get variable ID
    uint16_t id = *rxpayload;

    int length = 0;

    __Global_Variable_Types type;
    if (!get_global_type_from_id(id, &type)) return false;
    *txpayload = (uint8_t*)type;

    if (!get_global_name(id, txpayload + 1, &length, GNCLINK_PACKET_MAX_PAYLOAD_LENGTH - 1)) return false;

    if (!GNClink_Construct_Packet(txpacket, GNClink_PacketType_GetValueName, GNClink_PacketFlags_Response, length + 1)) return false;
    return true;
}

bool evaluatePacket() {
    // check packet
    if (!GNClink_Check_Packet(rxpacket)) {
        // SOS();
        // do something
        return false;
    }

    // // run buzzer
    // join_process(&bz);
    // reset_process(&bz);
    // bz_count = (int)(*GNClink_Get_Packet_Payload_Pointer(rxpacket));
    // dispatch_process(&bz);

    // check packet is not response packet
    GNClink_PacketFlags flags = GNClink_Get_Packet_Flags(rxpacket);
    if (flags & GNClink_PacketFlags_Response) return false;

    switch (GNClink_Get_Packet_Type(rxpacket)) {
        case GNClink_PacketType_GetGlobalHash:
        return getGlobalHash();

        case GNClink_PacketType_GetValueList:
        return getValueList();

        case GNClink_PacketType_SetValueList:
        return setValueList();

        case GNClink_PacketType_LoadValueList:
        return loadValueList();

        case GNClink_PacketType_SaveValueList:
        return saveValueList();

        case GNClink_PacketType_GetValueCount:
        return getValueCount();

        case GNClink_PacketType_GetValueName:
        return getValueName();

        default:
        return false;
    }

    return false;
}

bool get_frame() { // false if frame fails
    // Receive normally if port is VCP
    if (serial_is_usb_vcp(comms_port)) {
        // receive data
        serial_read_start(comms_port, rxframe, GNCLINK_FRAME_TOTAL_LENGTH);
        // wait until data arrives
        serial_read_wait_until_complete(comms_port); // Consider using _or_timeout in the future

        // check frame
        if (!GNClink_Check_Frame(rxframe)) {
            // continue, frame will be requested again later
            return false;
        }

        return true;
    }
    // read into infinite buffer if port is hardware serial
    else {
        rtos_delay_ms(10);

        for (int i = 0; i < sizeof(rxframe_buffer); ++i) {
            if (rxframe_buffer[i] == GNCLINK_FRAME_MAGIC) {
                // memcpy(crsf_cached_buffer, crsf_receive_buffer, sizeof(crsf_cached_buffer));
                for (int j = 0; j < GNCLINK_FRAME_TOTAL_LENGTH; ++j) {
                    rxframe[j] = rxframe_buffer[(i + j) % sizeof(rxframe_buffer)];
                }
                
                if (!GNClink_Check_Frame(rxframe)) {
                    continue;
                }

                // frame check passed, clear rxframe_buffer to avoid reusing same frame
                for (int j = 0; j < GNCLINK_FRAME_TOTAL_LENGTH; ++j) {
                    rxframe_buffer[(i + j) % sizeof(rxframe_buffer)] = 0; // reset read frame to zero
                }
                return true;
            }
        }
        return false;
    }
}

// returns false if resend is being requested
bool get_packet() {
    // set previous received packets to zero to avoid any propagation
    memset(rxpacket, 0, sizeof(rxpacket));
    memset(rxframe, 0, sizeof(rxframe));

    bool receivedFrames[GNCLINK_MAX_FRAMES_PER_PACKET];
    for (int i = 0; i < GNCLINK_MAX_FRAMES_PER_PACKET; ++i) receivedFrames[i] = false;

    // construct packet from frames
    while (1) {
        // // receive data
        // serial_read_start(comms_port, rxframe, GNCLINK_FRAME_TOTAL_LENGTH);
        // // wait until data arrives
        // serial_read_wait_until_complete(comms_port); // Consider using _or_timeout in the future
        // // led_on();
        // // rtos_delay_ms(1);
        // // led_off();

        // // check frame
        // if (!GNClink_Check_Frame(rxframe)) {
        //     // continue, frame will be requested again later
        //     continue;
        // }

        while (!get_frame()); // receive frame

        // check if frame is requesting resend
        if (GNClink_Frame_RequestResend(rxframe)) {
            return false;
        }

        // indicate frame has been received
        receivedFrames[GNClink_Get_Frame_Index(rxframe)] = true;

        // extract contents and place in packet
        bool moreFrames = true;
        GNClink_Reconstruct_Packet_From_Frames(rxframe, rxpacket, &moreFrames);

        // if no more frames, check if any need to be resent
        if (!moreFrames) {
            GNClink_FramePayload_RequestResend* payload = (GNClink_FramePayload_RequestResend*)GNClink_Get_Frame_Payload_Pointer(txframe);
            int resendCount = 0;
            for (int i = 0; i < GNClink_Get_Frame_Index(rxframe); ++i) {
                if (!receivedFrames[i]) payload->resendIndexes[resendCount++] = (uint8_t)i;
            }
            // if resend is required
            if (resendCount) {
                payload->resendCount = (uint8_t)resendCount;

                GNClink_Construct_RequestResendFrame(txframe);

                // send frame
#ifdef COMMS_TEST
                int random_value = rand() % 10;
                if (random_value != 0) {
#endif
                led_on(); // only flash if resend is being performed
                serial_write_start(comms_port, txframe, GNCLINK_FRAME_TOTAL_LENGTH);
                serial_write_wait_until_complete(comms_port);
                led_off();
#ifdef COMMS_TEST
                }
#endif
            }
            // packet fully received
            else break;
        }
    }
    return true;
}

bool send_packet(bool resendFrames) {
    int count = 0;
    bool moreFrames = true;
    while (moreFrames) {
        int frameIndex = count;
        GNClink_FrameFlags frameFlags = GNClink_FrameFlags_None;
        // check if frames are being resent
        if (resendFrames) {
            GNClink_FramePayload_RequestResend* payload = (GNClink_FramePayload_RequestResend*)GNClink_Get_Frame_Payload_Pointer(rxframe);
            // break if all have been sent
            if (count == payload->resendCount) break;
            if (count == payload->resendCount - 1) frameFlags |= GNClink_FrameFlags_TransactionEnd;
            frameIndex = payload->resendIndexes[count];
        }

        // send frames
        GNClink_Get_Frame(txpacket, txframe, frameFlags, frameIndex, &moreFrames);

        

        // send frame
#ifdef COMMS_TEST
        int random_value = rand() % 10;
        if (random_value != 0) {
#endif
        if (resendFrames) led_on(); // only flash LED if resend is being performed
        serial_write_start(comms_port, txframe, GNCLINK_FRAME_TOTAL_LENGTH);
        serial_write_wait_until_complete(comms_port);
        led_off();
#ifdef COMMS_TEST
        }
#endif

        ++count;
    }
    return true;
}

// bool comms_loop() {
//     bool run = true;
//     while (run) {
//         // get packet, resend if required
//         while (!get_packet()) {
//             send_packet(true);
//         }

//         if (evaluatePacket()) {
//             send_packet(false);
//         }
//     }

//     return true;
// }

bool comms_loop() {
    // get packet, resend if required
    while (!get_packet()) {
        send_packet(true);
    }

    if (evaluatePacket()) {
        send_packet(false);
    }

    return true;
}