#include "util.h"
#include "rc_receiver.h"
#include <string.h>


#define CRSF_BAUDRATE 420000
#define CRSF_SYNC_BYTE 0xC8
#define CRSF_CRC_POLY 0xD5

#define CRSF_FRAMETYPE_RC_CHANNELS_PACKED 0x16
#define CRSF_FRAMETYPE_LINK_STATISTICS 0x14

typedef struct __attribute__((packed)) {
    uint16_t ch0 : 11;
    uint16_t ch1 : 11;
    uint16_t ch2 : 11;
    uint16_t ch3 : 11;
    uint16_t ch4 : 11;
    uint16_t ch5 : 11;
    uint16_t ch6 : 11;
    uint16_t ch7 : 11;
    uint16_t ch8 : 11;
    uint16_t ch9 : 11;
    uint16_t ch10 : 11;
    uint16_t ch11 : 11;
    uint16_t ch12 : 11;
    uint16_t ch13 : 11;
    uint16_t ch14 : 11;
    uint16_t ch15 : 11;
} crsf_channels_type;

typedef struct __attribute__((packed)) {
    uint8_t uplink_rssi_ant1;
    uint8_t uplink_rssi_ant2;
    uint8_t uplink_linkqly;
    int8_t uplink_snr;
    uint8_t diversity_active_antenna;
    uint8_t rf_mode;
    uint8_t uplink_tx_power;
    uint8_t downlink_rssi;
    uint8_t downlink_linkqly;
    int8_t downlink_snr;
} crsf_link_statistics_type;

Process receiver_process;

uint8_t crsf_receive_buffer[64];
uint8_t crsf_cached_buffer[64];

crsf_channels_type crsf_channels;
crsf_link_statistics_type crsf_link_statistics;

// uint8_t crc8_calc(uint8_t crc, uint8_t a, uint8_t poly)
// {
//     crc ^= a;
//     for (int ii = 0; ii < 8; ++ii) {
//         if (crc & 0x80) {
//             crc = (crc << 1) ^ poly;
//         } else {
//             crc = crc << 1;
//         }
//     }
//     return crc;
// }

// #define crc8_dvb_s2(crc, a) crc8_calc(crc, a, 0xD5)

// uint8_t crsfFrameCRC(int index, uint8_t len)
// {
//     // CRC includes type and payload
//     // uint8_t crc = crc8_dvb_s2(0, crsfFrame.frame.type);
//     // for (int ii = 0; ii < crsfFrame.frame.frameLength - CRSF_FRAME_LENGTH_TYPE_CRC; ++ii) {
//     //     crc = crc8_dvb_s2(crc, crsfFrame.frame.payload[ii]);
//     // }
//     uint8_t crc = 0;
//     for (int i = 0; i < len - 1; ++i) {
//         crc = crc8_dvb_s2(crc, crsf_receive_buffer[(index + 2 + i) % 64]);
//     }

//     return crc;
// }

static const uint8_t crc8tab[256] = {
    0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54, 0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
    0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06, 0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
    0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0, 0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
    0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2, 0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
    0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9, 0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
    0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B, 0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
    0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D, 0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
    0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F, 0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
    0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB, 0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
    0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9, 0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
    0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F, 0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
    0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D, 0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
    0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26, 0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
    0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74, 0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
    0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82, 0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
    0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0, 0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9
};

uint8_t crc8_dvb_s2(int index, uint8_t len) {
    uint8_t crc = 0;
    for (uint8_t i = 0; i < len - 1; i++)
    {
        crc = crc8tab[crc ^ crsf_cached_buffer[(index + 2 + i) % 64]];
    }
    return crc;
}

bool rc_channels_packed(int index, uint8_t len) {
    uint8_t crc = crsf_cached_buffer[(index + len + 1) % 64];

    // check length
    if (len != 24) return false;

    // check crc
    if (crc8_dvb_s2(index, len) != crc) return false;

    for (int i = 0; i < sizeof(crsf_channels); ++i) {
        *((uint8_t*)&(crsf_channels) + i) = crsf_cached_buffer[(index + 3 + i) % 64];
    }

    globals.RCchannel1 = crsf_channels.ch0;
    globals.RCchannel2 = crsf_channels.ch1;
    globals.RCchannel3 = crsf_channels.ch2;
    globals.RCchannel4 = crsf_channels.ch3;
    globals.RCchannel5 = crsf_channels.ch4;
    globals.RCchannel6 = crsf_channels.ch5;
    globals.RCchannel7 = crsf_channels.ch6;
    globals.RCchannel8 = crsf_channels.ch7;
    globals.RCchannel9 = crsf_channels.ch8;
    globals.RCchannel10 = crsf_channels.ch9;
    globals.RCchannel11 = crsf_channels.ch10;
    globals.RCchannel12 = crsf_channels.ch11;
    globals.RCchannel13 = crsf_channels.ch12;
    globals.RCchannel14 = crsf_channels.ch13;
    globals.RCchannel15 = crsf_channels.ch14;
    globals.RCchannel16 = crsf_channels.ch15;

    return true;
}

bool link_statistics(int index, uint8_t len) {
    uint8_t crc = crsf_cached_buffer[(index + len + 1) % 64];

    // check length
    if (len != 12) return false;

    // check crc
    if (crc8_dvb_s2(index, len) != crc) return false;

    for (int i = 0; i < sizeof(crsf_link_statistics); ++i) {
        *((uint8_t*)&(crsf_link_statistics) + i) = crsf_cached_buffer[(index + 3 + i) % 64];
    }

    globals.RCRXFailsafe = crsf_link_statistics.uplink_linkqly == 0;
}

bool investigate_packet(int index) {
    uint8_t len = crsf_cached_buffer[(index + 1) % 64];
    uint8_t type = crsf_cached_buffer[(index + 2) % 64];

    switch (type) {
        case CRSF_FRAMETYPE_RC_CHANNELS_PACKED:
        return rc_channels_packed(index, len);

        case CRSF_FRAMETYPE_LINK_STATISTICS:
        return link_statistics(index, len);
        
        default:
        return false;
    }
}

void receiver_main() {
    while (1) {
        for (int i = 0; i < sizeof(crsf_receive_buffer); ++i) {
            if (crsf_receive_buffer[i] == CRSF_SYNC_BYTE) {
                memcpy(crsf_cached_buffer, crsf_receive_buffer, sizeof(crsf_cached_buffer));
                if (investigate_packet(i)) break;
            }
        }

        rtos_delay_ms(10);
    }
}

bool receiver_init(Serial_Port_IDs port) {
    // configure baud rate for CRSF
    // serial_set_baud(port, CRSF_BAUDRATE); // FIX TO NOT CRASH AFTER INIT

    // Start infinite receive buffer
	if (!serial_read_start_infinite(port, crsf_receive_buffer, sizeof(crsf_receive_buffer))) return false;

    // start up new process
    init_process(&receiver_process, receiver_main, RC_RECEIVER_STACK_BASE, RC_RECEIVER_STACK_SIZE);
    dispatch_process(&receiver_process);

    return true;
}