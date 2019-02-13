/**
 * stepper.c
 */


#include "stepper.h"

static char* TAG = "stepper_log_c"

/* Stepper motor half-step sequence 
 *
 *
 *   State A B C D
 *     1   1 0 0 0
 *     2   1 1 0 0
 *     3   0 1 0 0
 *     4   0 1 1 0
 *     5   0 0 1 0
 *     6   0 0 1 1
 *     7   0 0 0 1
 *     8   1 0 0 1
 *
 */

uint_ stepperPinA, stepperPinB, stepperPinC, stepperPinD;

GPIO_OUTPUT_BITMASK = 

ESP_LOGI(TAG, "GPIO config");
gpio_config_t io_conf;
io_conf.intr_type = GPIO_INTR_DISABLE;
io_conf.mode = GPIO_MODE_OUTPUT;
io_conf.pin_bit_mask = GPIO_OUTPUT_BITMASK;
io_conf.pull_down_en = 0;
io_conf.pull_up_en = 0;
gpio_config(&io_conf);

