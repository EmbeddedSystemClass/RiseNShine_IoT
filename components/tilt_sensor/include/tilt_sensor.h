/**
 * Module for tilt sensor
 */

#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"

typedef enum {
    GPIO_LEVEL_LOW = 0,
    GPIO_LEVEL_HIGH
} gpio_level_t;

void tiltsens_initTiltSensorPin();
gpio_level_t tiltsens_getSensor1Level();
gpio_level_t tiltsens_getSensor2Level();