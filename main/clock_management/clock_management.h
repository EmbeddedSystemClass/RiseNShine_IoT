/*
clock_management.h


System clock management

This module contains the hardware timer used to keep track of the system time using hour, minutes and seconds.

*/

#include "esp_system.h"

static const char* TAG = "clock_management_log";

typedef struct stime_t {
	unsigned int hour = 0;
	unsigned int minute = 0;
	unsigned int second = 0;
} time_t;

void vTaskClockSystem(void *pvParameters);

