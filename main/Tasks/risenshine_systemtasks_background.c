/**
 * risenshine_systemtasks_background
 * 
 * Task implementation for low-priority tasks
 */

#include "risenshine_systemtasks.h"

#include "http_get.h"

#include "esp_log.h"

static const char * TAG = "Background task";


void vTaskIdleComputations(void *pvParameters)
{
    while(1)
    {
        http_get_task(NULL);
    }
    
    
}