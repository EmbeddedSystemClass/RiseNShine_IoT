/**
 * risenshine_systemtasks.h
 * 
 * APIs for tasks that exists in the system. Synthesis and
 * integration of multi-module tasks occurs here
 * 
 */

#pragma once

void vTaskClockSystem(void* pvParameters);
void vTaskTCPServer(void* pvParameters);
void vTaskStepperMotorControl(void *pvParameters);
void vTaskIdleComputations(void *pvParameters);