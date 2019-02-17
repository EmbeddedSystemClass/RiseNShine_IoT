/**
 * risenshine_systemtasks_clock.c
 *
 * Task to manage the system clocks and timings. 
 */

#include "risenshine_systemtasks.h"

#include "esp_log.h"
#include "clock_management.h"

static const char* TAG = "clock_management_log";

void vTaskClockSystem(void *pvParameters)
{

	//initialization
	ESP_LOGI(TAG, "Init of time struct");
	const TickType_t xDelayDuration = 1000 / portTICK_PERIOD_MS;
	static timeFormat_t current_time;
	static timeFormat_t sunset_time;
	static timeFormat_t sunrise_time;

	set_time(&current_time, 0,0,0);
	set_time(&sunset_time, 18,0,0);
	set_time(&sunrise_time, 6, 0, 0);
	//ESP_LOGD(TAG, "Current time: %d:%d:%d", current_time.hour, current_time.minute,current_time.second);


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

	//int cnt = 0;

	ESP_LOGI(TAG, "Start Task");
	//start task
	for(;;)
	{
		increment_time(&current_time);
		//gpio_set_level(GPIO_LED_OUTPUT_IO, ++cnt % 2);
		ESP_LOGI(TAG, "Current time: %d:%d:%d", current_time.hour, current_time.minute,current_time.second);


		if(current_time.second == 0)
		{
			if(compare_time(&current_time, &sunrise_time) == 1)
			{
				//send command to open blinds
			}
		}
		
		if(current_time.second == 30) 
		{
			if(compare_time(&current_time, &sunset_time) == 1)
			{
				//send command to close blinds
			}
		}


		//check q to see if current time has a new update
		/*if ( xQueueReceive( asdf ))
		{
			set_time(current_time, asdf)
		}*/


		vTaskDelay(xDelayDuration); //repeat every 1 second
	}


	// Never gets here!
}