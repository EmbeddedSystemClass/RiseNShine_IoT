/*
clock_management.h


System clock management

This module contains the hardware timer used to keep track of the system time using hour, minutes and seconds.

*/

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

typedef struct stime_t {
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
} timeFormat_t;

void vTaskClockSystem(void *pvParameters);

