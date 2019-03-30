/*
 * clock_management.h
 * This module contains the clock related functions used to keep track 
 * of the system time using hour, minutes and seconds. It also contains
 * functions to manage the sunrise and sunset times.
 */

#pragma once

#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define GPIO_LED_OUTPUT_IO 2 //pin 2 - built in LED on NodeMCU
#define GPIO_OUTPUT_BITMASK (1ULL << GPIO_LED_OUTPUT_IO)

typedef struct {
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
} timeFormat_t;

typedef enum {
	CLOCK_CURRENTTIME = 0,
	CLOCK_SUNSETTIME,
	CLOCK_SUNRISETIME
} clockType_e;

bool clock_compareTime(clockType_e clock1, clockType_e clock2);
bool clock_compareToCurrentTime(clockType_e clock1);
void clock_incrementCurrentTime();
bool clock_setTime(clockType_e clockType, timeFormat_t *newTime);
void clock_getTime(clockType_e clockType, timeFormat_t *const data);