/**
 * stepper.h
 * 
 * Stepper driver for 28BYJ-48 stepper motor. Could also possibly
 * be used for other unipolar motors.
 * 
 ** Stepper motor full-step sequence 
 *   State A B C D
 *     1   1 0 0 0
 *     2   0 1 0 0
 *     3   0 0 1 0
 *     4   0 0 0 1
 * 
 ** Stepper motor half-step sequence
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

#pragma once

#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define stepperPinA 14 // D5
#define stepperPinB 12 // D6
#define stepperPinC 13 // D7
#define stepperPinD 15 // D8

#define GPIO_STEPPER_OUTPUT_BITMASK ( (1ULL<<stepperPinA) | \
                                      (1ULL<<stepperPinB) | \
                                      (1ULL<<stepperPinC) | \
                                      (1ULL<<stepperPinD) )

/**
 * Enums used to describe the different states
 * of the stepper motor
 */
typedef enum 
{
    //for output state
    OFF = 0,
    ON,

    //for stepper states
    stateA = 0,
    stateB,
    stateC,
    stateD,

    //for direction
    CLOCKWISE = 0,
    COUNTCLOCKWISE
} state_e;

void initStepperPins();
void moveStepper(int stepsnum);
void stopStepper();
void changeStepperDirection();