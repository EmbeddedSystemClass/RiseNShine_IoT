/**
 * stepper.c
 */


#include "stepper.h"

static char* TAG = "stepper_log_c";

/* Stepper motor full-step sequence 
 *
 *
 *   State A B C D
 *     1   1 0 0 0
 *     2   0 1 0 0
 *     3   0 0 1 0
 *     4   0 0 0 1
 *
 */

typedef enum {
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

#define stepperPinA 14 // D5
#define stepperPinB 12 // D6
#define stepperPinC 13 // D7
#define stepperPinD 15 // D8

#define GPIO_STEPPER_OUTPUT_BITMASK ( (1ULL<<stepperPinA) | \
                                      (1ULL<<stepperPinB) | \
                                      (1ULL<<stepperPinC) | \
                                      (1ULL<<stepperPinD) )

// Static global vars
static state_e cwDirection = CLOCKWISE;
static state_e stepperState = stateA;
static int numStepsLeft = 0;

void initStepperPins() {
    ESP_LOGI(TAG, "Stepper GPIO config");
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_STEPPER_OUTPUT_BITMASK;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}

void moveStepper(int stepsnum) {
    numStepsLeft += stepsnum;
}

void stopStepper() {
    numStepsLeft = 0;
}

void changeStepperDirection() {
    cwDirection = !cwDirection;
}

static void changePinOutputs(state_e A, state_e B, state_e C, state_e D) {
    gpio_set_level(stepperPinA, A);
    gpio_set_level(stepperPinB, B);
    gpio_set_level(stepperPinC, C);
    gpio_set_level(stepperPinD, D);
}

static void applyState() {
    if (numStepsLeft != 0) {
        switch(stepperState) {
            case stateA:
                changePinOutputs(ON , OFF, OFF, OFF);
                break;
            case stateB:
                changePinOutputs(OFF, ON , OFF, OFF);
                break;
            case stateC:
                changePinOutputs(OFF, OFF, ON , OFF);
                break;
            case stateD:
                changePinOutputs(OFF, OFF, OFF, ON );
                break;
            default:
                changePinOutputs(OFF, OFF, OFF, OFF);
        }
    }
}

// @todo how to increment state stored as enums?
static void changeState() {
    if (cwDirection == CLOCKWISE) {
        stepperState++;
        if (stepper)
    }
}

