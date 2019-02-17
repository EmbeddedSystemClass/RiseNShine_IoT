/**
 * risenshine_systemtasks_steppercontrol
 * 
 * Task implementation for the stepper motor
 */

#include "risenshine_systemtasks.h"

#include "stepper.h"

#define STEPPER_FREQUENCY_MS 50

void vTaskStepperMotorControl(void *pvParameters)
{
    initStepperPins();
    const TickType_t xDelayDuration = STEPPER_FREQUENCY_MS / portTICK_PERIOD_MS;
    int numStepsToAdd = 0;
    while(1) 
    {
        if (xQueueReceive(qStepperMotorSteps,numStepsToAdd,0) == pdTRUE)
        {
            stepper_moveStepper(numStepsToAdd);
        }
        applyState();
        vTaskDelay(xDelayDuration);
    }
}