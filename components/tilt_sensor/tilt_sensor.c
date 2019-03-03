/**
 * Module for tilt sensor
 */

#include "tilt_sensor.h"

#define CONVERTTOBITMASK(x) (1ULL<<x)

#define IO_INPUT_PIN_TILTSENSOR1 5 // D1
#define IO_INPUT_PIN_TILTSENSOR2 4 // D2

#define GPIO_TILT_SENSOR_INPUT_BITMASK ( (1ULL << IO_INPUT_PIN_TILTSENSOR1) | \
                                         (1ULL << IO_INPUT_PIN_TILTSENSOR2) )

static const char* TAG = "Tilt Sensor";

void tiltsens_initTiltSensorPin()
{
    ESP_LOGI(TAG, "Tilt Sensor pins init");
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_TILT_SENSOR_INPUT_BITMASK;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
}

gpio_level_t tiltsens_getSensor1Level()
{
    int level = gpio_get_level(GPIO_NUM_5);
    switch(level)
    {
        case 0:
            return GPIO_LEVEL_LOW;
        case 1:
            return GPIO_LEVEL_HIGH;
        default:
            return GPIO_LEVEL_LOW;
    }
}

gpio_level_t tiltsens_getSensor2Level()
{
    int level = gpio_get_level(GPIO_NUM_4);
    switch(level)
    {
        case 0:
            return GPIO_LEVEL_LOW;
        case 1:
            return GPIO_LEVEL_HIGH;
        default:
            return GPIO_LEVEL_LOW;
    }
}