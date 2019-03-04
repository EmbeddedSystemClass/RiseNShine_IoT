/**
 * risenshine_systemtasks_steppercontrol
 * 
 * Task implementation for the stepper motor
 */

#include "risenshine_systemtasks.h"

#include "stepper.h"
#include "driver/hw_timer.h"

#define STEPPER_FREQUENCY_uS 10000 // 100 Hz = 1 / 10000 us
#define TIMER_AUTORELOAD_TRUE true
#define BLINDSNUMSTEPS 3000

static bool isBlindsOpen = false;

static void openBlinds()
{
    if(!isBlindsOpen) 
    {
        stepper_setStepperDirection(CLOCKWISE);
        stepper_moveStepper()
    }
}

static void initializeBlindsPosition()
{
    stepper_setStepperDirection(COUNTCLOCKWISE);

}

void vTaskStepperMotorControl(void *pvParameters)
{
    stepper_initStepperPins();
    //const TickType_t xDelayDuration = STEPPER_FREQUENCY_MS / portTICK_PERIOD_MS;
    int numStepsToAdd = 0;

    hw_timer_init(stepper_applyState, NULL);
    hw_timer_alarm_us(STEPPER_FREQUENCY_uS, TIMER_AUTORELOAD_TRUE);
    while(1) 
    {
        if (xQueueReceive(qStepperMotorSteps,&numStepsToAdd,portMAX_DELAY) == pdTRUE)
        {
            stepper_moveStepper(numStepsToAdd);
        }
    }
}