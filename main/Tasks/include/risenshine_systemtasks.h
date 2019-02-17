/**
 * risenshine_systemtasks.h
 * 
 * APIs for tasks that exists in the system. Synthesis and
 * integration of multi-module tasks occurs here
 * 
 */

#pragma once

#include "clock_management.h"
#include "TCP_server.h"
#include "connect_to_wifi.h"
#include "stepper.h"

void vTaskClockSystem(void* pvParameters);
void vTaskTCPServer(void* pvParameters);
void vTaskStepperMotorControl(void *pvParameters);
void vTaskIdleComputations(void *pvParameters);