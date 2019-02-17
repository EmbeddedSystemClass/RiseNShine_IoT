/**
 * connect_to_wifi.h
 * 
 * Module to connect to wifi. Configuration is based on
 * the menuconfig as specified in the KConfig
 */

#pragma once

#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

void initialise_wifi(void);
void wait_for_ip();