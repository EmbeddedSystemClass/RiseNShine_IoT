/**
 * risenshine_systemtasksconfig.h
 * 
 * Header files to contain the different tasks config for the RTOS
 */

#pragma once

#include "Tasks/include/risenshine_systemtasks.h"


// Priority Levels
#define CLOCK_MANAGEMENT_PRIORITY 10
#define TCP_SERVER_PRIORITY 9
#define STEPPER_CONTROL_PRIORITY 8

// Stack size config
#define CLOCK_MANAGEMENT_STACKSIZE 2048
#define TCP_SERVER_STACKSIZE 4096
#define STEPPER_CONTROL_STACKSIZE 2048


/***************************/
/*** Tasks configuration ***/
/***************************/

/**
 * Struct to hold tasks configuration
 */
typedef struct {
    TaskFunction_t functionCall;
    const char * const taskName;
    const configSTACK_DEPTH_TYPE sizeOfStackinWords; // Stack size = word (32bits for esp 8266) * numOfWords
    void* const pvTaskParameters;
    UBaseType_t uxPriority;
    TaskHandle_t* const pxCreatedTask;
} TaskParametersList_t;

/**
 * Wrapper for xTaskCreate to pass structs as arguments
 */
BaseType_t xTaskCreate_wrapper(TaskParametersList_t taskParameters)
{
    return xTaskCreate( taskParameters.functionCall,        \
                        taskParameters.taskName,            \
                        taskParameters.sizeOfStackinWords,  \
                        taskParameters.pvTaskParameters,    \
                        taskParameters.uxPriority,          \
                        taskParameters.pxCreatedTask);
}


/* Clock Management Task */
TaskParametersList_t s_clockMgtTask = { 
    .functionCall=        vTaskClockSystem,
    .taskName=            "clock_management_task",
    .sizeOfStackinWords=  CLOCK_MANAGEMENT_STACKSIZE,
    .pvTaskParameters=    NULL,
    .uxPriority=          CLOCK_MANAGEMENT_PRIORITY,
    .pxCreatedTask=       NULL 
};

/* TCP Server Task */
TaskParametersList_t s_TCPServerTask = { 
    .functionCall=        vTaskTCPServer,
    .taskName=            "tcp_server_task",
    .sizeOfStackinWords=  TCP_SERVER_STACKSIZE,
    .pvTaskParameters=    NULL,
    .uxPriority=          TCP_SERVER_PRIORITY,
    .pxCreatedTask=       NULL  
};

TaskParametersList_t s_StepperControlTask = { 
    .functionCall=        vTaskStepperMotorControl,
    .taskName=            "stepper_motor_control",
    .sizeOfStackinWords=  STEPPER_CONTROL_STACKSIZE,
    .pvTaskParameters=    NULL,
    .uxPriority=          STEPPER_CONTROL_PRIORITY,
    .pxCreatedTask=       NULL  
};

/**
 * Task Struct Template
 * 
 * 
 * @code

TaskParametersList_t s_Task = { 
    .functionCall=        ,
    .taskName=            ,
    .sizeOfStackinWords=  _STACKSIZE,
    .pvTaskParameters=    NULL,
    .uxPriority=          _PRIORITY,
    .pxCreatedTask=       NULL  
};

 */

