/**
 *  clock_management_test.c
 *  
 *	Driver program to test clock_management.h
 */

#include "esp_system.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

#include "clock_management.h"

static const char *TAG = "clock_mgmt_test";

#define GPIO_LED_OUTPUT_IO (1ULL << 12) //pin12


void app_main(void)
{
    //ESP_LOGI(TAG, "GPIO config");
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_LED_OUTPUT_IO;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    xTaskCreate(vTaskClockSystem, "clock_management_task", 2048, NULL, 10, NULL)

}
