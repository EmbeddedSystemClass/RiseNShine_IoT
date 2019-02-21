/**
 * risenshine_systemtasks.h
 * 
 * APIs for tasks that exists in the system. Synthesis and
 * integration of multi-module tasks occurs here
 * 
 */

#pragma once

#include "FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "clock_management.h"

// Queues
QueueHandle_t qStepperMotorSteps;
QueueHandle_t qClockUpdate;

void vTaskClockSystem(void* pvParameters);
void vTaskTCPServer(void* pvParameters);
void vTaskStepperMotorControl(void *pvParameters);
void vTaskIdleComputations(void *pvParameters);

// shared data structures
typedef enum {
    currentTime = 0,
    sunsetTime,
    sunriseTime,
} timeType_e;

typedef struct {
    timeType_e timeType;
    timeFormat_t timeData;
} timePackage_t;