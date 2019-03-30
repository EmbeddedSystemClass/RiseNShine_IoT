/**
 * risenshine_systemtasks_steppercontrol
 * 
 * Task implementation for the stepper motor
 */

#include "risenshine_systemtasks.h"

#include "stepper.h"
#include "driver/hw_timer.h"
#include "esp_log.h"

static const char * TAG = "Stepper control task";

#define STEPPER_FREQUENCY_uS 10000 // 100 Hz = 1 / 10000 us
#define TIMER_AUTORELOAD_TRUE true
#define BLINDSNUMSTEPS 10000

/*  To open blinds: go CCW
    To close blinds: go CW */

// until tilt sensor is operational, initial state of the blinds must be CLOSED
static volatile bool isBlindsOpen = false;

static void openBlinds()
{
    if(!isBlindsOpen) 
    {
        stepper_setStepperDirection(COUNTCLOCKWISE);
        stepper_moveStepper(BLINDSNUMSTEPS);
        isBlindsOpen = true;
    }
}

static void closeBlinds()
{
    if(isBlindsOpen)
    {
        stepper_setStepperDirection(CLOCKWISE);
        stepper_moveStepper(BLINDSNUMSTEPS);
        isBlindsOpen = false;
    }
}


//need tilt sensor to use this
static void initializeBlindsPosition()
{
    //stepper_setStepperDirection(COUNTCLOCKWISE);
    return;
}

static void parseStepCommand(stepCmd_e cmd)
{
    switch(cmd)
    {
        case STEPCMD_OPENBLINDS:
            openBlinds();
            break;
        case STEPCMD_CLOSEBLINDS:
            closeBlinds();
            break;
        case STEPCMD_STOP:
            stepper_stopStepper();
            break;
        default:
            ESP_LOGE(TAG, "Error: Unknown stepper command");
            return;
    }
}

void vTaskStepperMotorControl(void *pvParameters)
{
    stepper_initStepperPins();
    initializeBlindsPosition();

    stepCmd_e stepCommand;

    hw_timer_init(stepper_applyState, NULL);
    hw_timer_alarm_us(STEPPER_FREQUENCY_uS, TIMER_AUTORELOAD_TRUE);
    while(1)
    {
        if (xQueueReceive(qStepperCommands,&stepCommand,portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI(TAG, "Received command: %d", stepCommand);
            ESP_LOGI(TAG, "Blind status: %d", isBlindsOpen);
            parseStepCommand(stepCommand);
        }
    }
}