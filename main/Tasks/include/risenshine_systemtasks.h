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

/// @todo: Issue #14 get these outta here
// Queues
QueueHandle_t qStepperCommands;
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

typedef enum {
    STEPCMD_OPENBLINDS = 0,
    STEPCMD_CLOSEBLINDS,
    STEPCMD_STOP,
    STEPCMD_INVALID
} stepCmd_e;

typedef struct {
    timeType_e timeType;
    timeFormat_t timeData;
} timePackage_t;