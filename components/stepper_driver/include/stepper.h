/**
 * stepper.h
 */

 #pragma once

/* Stepper motor half-step sequence 
 *
 *
 *   State A B C D
 *     1   1 0 0 0
 *     2   1 1 0 0
 *     3   0 1 0 0
 *     4   0 1 1 0
 *     5   0 0 1 0
 *     6   0 0 1 1
 *     7   0 0 0 1
 *     8   1 0 0 1
 *
 */

#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"

void initStepperPins();
void moveStepper(int stepsnum);
void stopStepper();
void changeStepperDirection();