
#include <stdio.h>

#include "esp_system.h"

#include "connect_to_wifi.h"
#include "risenshine_systemtasksconfig.h"

#include "nvs_flash.h" // for nvs_flash_init()

void app_main()
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    initialise_wifi();
    wait_for_ip();

    //init queues
    qStepperCommands = xQueueCreate(2, sizeof(stepCmd_e));

    xTaskCreate_wrapper(s_clockMgtTask);
    xTaskCreate_wrapper(s_TCPServerTask);
    xTaskCreate_wrapper(s_StepperControlTask);
    xTaskCreate_wrapper(s_IdleComputationTask);
}
