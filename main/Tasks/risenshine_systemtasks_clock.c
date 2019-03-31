/**
 * risenshine_systemtasks_clock.c
 *
 * Task to manage the system clocks and timings. 
 */

#include "risenshine_systemtasks.h"

#include "esp_log.h"
#include "clock_management.h"
#include "driver/gpio.h"

#define TICK_INTERVAL_MS 1000

//GPIO
#define GPIO_LED_OUTPUT_IO 2 //pin 2 - built in LED on NodeMCU
#define GPIO_OUTPUT_BITMASK (1ULL << GPIO_LED_OUTPUT_IO)

static const char* TAG = "clock_management_log";
static const TickType_t queueDelayMs = 500 / portTICK_PERIOD_MS;

void vTaskClockSystem(void *pvParameters)
{

	//initialization
	ESP_LOGI(TAG, "Init of time struct");
	const TickType_t xDelayDuration = TICK_INTERVAL_MS / portTICK_PERIOD_MS;

	// init IO pin
	ESP_LOGI(TAG, "GPIO config");
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_BITMASK;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
	gpio_set_level(GPIO_LED_OUTPUT_IO, 0); //only visual indication that wifi is ready

	stepCmd_e stepCommand = STEPCMD_INVALID;
	//int cnt = 0;

	ESP_LOGI(TAG, "Start Task");
	for(;;)
	{
		clock_incrementCurrentTime();
		timeFormat_t currentTime_local;
		clock_getTime(CLOCK_CURRENTTIME, &currentTime_local);
		//gpio_set_level(GPIO_LED_OUTPUT_IO, ++cnt % 2)
		//ESP_LOGI(TAG, "Current time: %02d:%02d:%02d", currentTime_local.hour, currentTime_local.minute, currentTime_local.second);

		//check every minute
		if(currentTime_local.second == 0)
		{
			if(clock_compareTime(CLOCK_CURRENTTIME, CLOCK_SUNRISETIME) == true)
			{
				stepCommand = STEPCMD_OPENBLINDS;
				xQueueSend(qStepperCommands, &stepCommand, queueDelayMs);
			}
		}
		
		//check every minute
		if(currentTime_local.second == 30) 
		{
			if(clock_compareTime(CLOCK_CURRENTTIME, CLOCK_SUNSETTIME) == true)
			{
				stepCommand = STEPCMD_CLOSEBLINDS;
				xQueueSend(qStepperCommands, &stepCommand, queueDelayMs);
			}
		}
		
		vTaskDelay(xDelayDuration); //repeat every 1 second
	}
	// Never gets here!
}