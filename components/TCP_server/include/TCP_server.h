/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once

#include <string.h>
#include <sys/param.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#define PORT CONFIG_EXAMPLE_PORT

bool tcp_createAndBindSocket(int * listen_sock);
bool tcp_acceptClients(int listen_sock);
bool tcp_sendMessage(int socket, char* payload, uint32_t len);
bool tcp_recvMessage(int socket, char* msgBuffer, uint32_t msgBufferSize);
bool tcp_closeSocket(int socket);
