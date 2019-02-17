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

// Queues
QueueHandle_t qStepperMotorSteps;

void vTaskClockSystem(void* pvParameters);
void vTaskTCPServer(void* pvParameters);
void vTaskStepperMotorControl(void *pvParameters);
void vTaskIdleComputations(void *pvParameters);