/*
clock_management.h


System clock management

This module contains the hardware timer used to keep track of the system time using hour, minutes and seconds.

*/

#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define GPIO_LED_OUTPUT_IO 2 //pin 2 - built in LED on NodeMCU
#define GPIO_OUTPUT_BITMASK (1ULL << GPIO_LED_OUTPUT_IO)

typedef struct stime_t {
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
} timeFormat_t;

void vTaskClockSystem(void *pvParameters);

