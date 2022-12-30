#include "comms.h"
#include "gnclink.h"
#include "globals.h"
#include "global_ports.h"
#include "RTOS/rtos.h"
#include PORT_DRIVER

#include <string.h>

uint8_t rxpacket[GNCLINK_PACKET_MAX_TOTAL_LENGTH];
uint8_t txpacket[GNCLINK_PACKET_MAX_TOTAL_LENGTH];
uint8_t rxframe[GNCLINK_FRAME_TOTAL_LENGTH];
uint8_t txframe[GNCLINK_FRAME_TOTAL_LENGTH];

extern void SOS();
extern Process bz;
extern int bz_count;

bool getGlobalHash() {
    uint32_t* payload = (uint32_t*)GNClink_Get_Packet_Payload_Pointer(txpacket);
    *payload = __GLOBAL_HASH;
    if (!GNClink_Construct_Packet(txpacket, GNClink_PacketType_GetGlobalHashResponse, GNClink_PacketFlags_None, 4)) return false;
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

    // construct packet
    if (!GNClink_Construct_Packet(txpacket, GNClink_PacketType_GetValueListResponse, GNClink_PacketFlags_None, payloadIndex)) return false;
    return true;
}

bool evaluatePacket() {
    // check packet
    if (!GNClink_Check_Packet(rxpacket)) {
        // do something
        SOS();
    }

    // // run buzzer
    // join_process(&bz);
    // reset_process(&bz);
    // bz_count = (int)(*GNClink_Get_Packet_Payload_Pointer(rxpacket));
    // dispatch_process(&bz);

    switch (GNClink_Get_Packet_Type(rxpacket)) {
        case GNClink_PacketType_GetGlobalHash:
        return getGlobalHash();

        case GNClink_PacketType_GetValueList:
        return getValueList();

        default:
        return false;
    }

    return false;
}

// returns false if resend is being requested
bool get_packet() {
    bool receivedFrames[GNCLINK_MAX_FRAMES_PER_PACKET];
    for (int i = 0; i < GNCLINK_MAX_FRAMES_PER_PACKET; ++i) receivedFrames[i] = false;

    // construct packet from frames
    while (1) {
        // receive data
        serial_read_start(PORT0, rxframe, GNCLINK_FRAME_TOTAL_LENGTH);
        // wait until data arrives
        serial_read_wait_until_complete(PORT0);

        // check frame
        if (!GNClink_Check_Frame(rxframe)) {
            // continue, frame will be requested again later
            continue;
        }

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
                serial_write_start(PORT0, txframe, GNCLINK_FRAME_TOTAL_LENGTH);
                serial_write_wait_until_complete(PORT0);
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
        serial_write_start(PORT0, txframe, GNCLINK_FRAME_TOTAL_LENGTH);
        serial_write_wait_until_complete(PORT0);

        ++count;
    }
    return true;
}

bool comms_loop() {
    bool run = true;
    while (run) {
        // get packet, resend if required
        while (!get_packet()) {
            send_packet(true);
        }

        if (evaluatePacket()) {
            send_packet(false);
        }
    }

    return true;
}