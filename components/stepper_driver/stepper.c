/**
 * stepper.c
 */


#include "stepper.h"

static char* TAG = "stepper_log_c";
static stepperState_e cwDirection = CLOCKWISE;
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
 * Gets if the stepper currently has steps to be run
 *
 * @param bool true if there's steps to be done
 *             false if there's none
 */
inline bool stepper_isRunning()
{
    if(numStepsLeft != 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * Place stepper in idle mode by turning off all GPIO pins
 * 
 * @return bool true if state is changed, false if there's still steps
 */
bool stepper_idleMode()
{
    if(stepper_isRunning())
    {
        stepperState = stateIdle;
        return true;
    }
    return false;
}

/**
 * Set the direction of the stepper
 * 
 * @return If valid direction, returns true
 *         else, false
 */
bool stepper_setStepperDirection(stepperState_e newDirection)
{
    switch (newDirection)
    {
        case CLOCKWISE:
            cwDirection = CLOCKWISE;
            return true;
            break;
        case COUNTCLOCKWISE:
            cwDirection = COUNTCLOCKWISE;
            return true;
            break;
        default:
            return false;
            break;
    }
}

/**
 * Changes the direction of the current stepper direction
 */
void stepper_changeStepperDirection() 
{
    cwDirection = !cwDirection;
}

static void changePinOutputs(stepperState_e A, stepperState_e B, stepperState_e C, stepperState_e D) 
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

static void turnOffPins()
{
    changePinOutputs(OFF, OFF, OFF, OFF);
}

/**
 * Applies the new state by accessing the GPIO pins
 */
void stepper_applyState() 
{
    if (stepper_isRunning()) 
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
    else
    {
        turnOffPins();
    }
}



