/**
 * stepper.c
 */


#include "stepper.h"

static char* TAG = "stepper_log_c";
static state_e cwDirection = CLOCKWISE;
static int stepperState = (int)stateA;
static uint numStepsLeft = 0;

/**
 * Initializes the GPIO pins for the stepper motors. Pins
 * are defined in stepper.h. It's on the developer to ensure
 * there's no pin conflict
 */
void stepper_initStepperPins() 
{
    ESP_LOGI(TAG, "Stepper GPIO config");
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_STEPPER_OUTPUT_BITMASK;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}

/**
 * Increases the amount of steps queued for the stepper  
 * 
 * @param stepsnum - number of steps to add
 */
void stepper_moveStepper(int stepsnum) 
{
    numStepsLeft += stepsnum;
}

/**
 * Clears the number steps queue
 */
void stepper_stopStepper() 
{
    numStepsLeft = 0;
}

/**
 * Changes the direction of the current stepper direction
 */
void stepper_changeStepperDirection() 
{
    cwDirection = !cwDirection;
}

static void changePinOutputs(state_e A, state_e B, state_e C, state_e D) 
{
    gpio_set_level(stepperPinA, A);
    gpio_set_level(stepperPinB, B);
    gpio_set_level(stepperPinC, C);
    gpio_set_level(stepperPinD, D);
}

static void changeState() 
{
    if (cwDirection == CLOCKWISE) 
    {
        stepperState++;
        if (stepperState == ((int)stateD+1)) 
        {
            stepperState = (int)stateA;
        }
    }
    else if (cwDirection == COUNTCLOCKWISE) 
    {
        stepperState--;
        if (stepperState ==((int)stateA-1))
        {
            stepperState = (int)stateD;
        }
    }
}

/**
 * Applies the new state by accessing the GPIO pins
 */
void stepper_applyState() 
{
    if (numStepsLeft != 0) 
    {
        changeState();
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
        numStepsLeft--;
    }
}



