/* HTTP GET Example using plain POSIX sockets

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <string.h>
#include "esp_log.h"
#include "cJSON.h"
#include <netdb.h>
#include "clock_management.h"

/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "api.sunrise-sunset.org"
#define WEB_PORT 80
#define WEB_URL "http://api.sunrise-sunset.org/json?lat=49.2827&lng=-123.1207&formatted=0"

typedef struct {
    timeFormat_t sunriseTime;
    timeFormat_t sunsetTime;
} strSunriseSunsetTimes_t;

int http_connectSocket();
void http_sendRequest(int s, char payload[], int payloadSize);