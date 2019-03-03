/* 
 * get_ntp.h
 * 
 **/

#pragma once

#include <string.h>
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>


typedef struct {
    uint8_t li_vn_mode;      // Eight bits. li, vn, and mode. [0]
                            // li.   Two bits.   Leap indicator. 
                            // vn.   Three bits. Version number of the protocol. 
                            // mode. Three bits. Client will pick mode 3 for client. 

    uint8_t stratum;         // Eight bits. Stratum level of the local clock. [1]
    uint8_t poll;            // Eight bits. Maximum interval between successive messages. [2]
    uint8_t precision;       // Eight bits. Precision of the local clock.[3]

    uint32_t rootDelay;      // 32 bits. Total round trip delay time. [4-7]
    uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source. [8-11]
    uint32_t refId;          // 32 bits. Reference clock identifier. [12-15]

    uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds. [16-19]
    uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second. [20-23]

    uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds. [24-27]
    uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second. [28-31]

    uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds. [32-35]
    uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second. [36-39]

    uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
                             // [40-43]
    uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.  [44-47]
} ntp_packet_t;

int udp_getUDPsocket();
void udp_sendMsg(int sock);
void udp_recvMsg(int sock, void(*call_back_ptr)(uint32_t *data));