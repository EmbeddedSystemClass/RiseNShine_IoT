/**
 * risenshine_systemtasks_clock.c
 *
 * Task to manage the system clocks and timings. 
 */

#include "risenshine_systemtasks.h"

#include "esp_log.h"
#include "clock_management.h"

#define TICK_INTERVAL_MS 1000

static const char* TAG = "clock_management_log";
static const TickType_t queueDelayMs = 500 / portTICK_PERIOD_MS;

void vTaskClockSystem(void *pvParameters)
{

	//initialization
	ESP_LOGI(TAG, "Init of time struct");
	const TickType_t xDelayDuration = TICK_INTERVAL_MS / portTICK_PERIOD_MS;
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

	timePackage_t recvTimePackage;
	stepCmd_e stepCommand = STEPCMD_INVALID;
	//int cnt = 0;

	ESP_LOGI(TAG, "Start Task");
	//start task
	for(;;)
	{
		increment_time(&current_time);
		//gpio_set_level(GPIO_LED_OUTPUT_IO, ++cnt % 2);
		//ESP_LOGI(TAG, "Current time: %d:%d:%d", current_time.hour, current_time.minute,current_time.second);

		//check every minute
		if(current_time.second == 0)
		{
			if(compare_time(&current_time, &sunrise_time) == true)
			{
				stepCommand = STEPCMD_OPENBLINDS;
				xQueueSend(qStepperCommands, &stepCommand, queueDelayMs);
			}
		}
		
		//check every minute
		if(current_time.second == 30) 
		{
			if(compare_time(&current_time, &sunset_time) == true)
			{
				stepCommand = STEPCMD_CLOSEBLINDS;
				xQueueSend(qStepperCommands, &stepCommand, queueDelayMs);
			}
		}


		//check q to see if current time has a new update
		if ( xQueueReceive(qClockUpdate, &recvTimePackage, 0))
		{
			switch(recvTimePackage.timeType)
			{
				case sunsetTime:
					set_time(&sunset_time, recvTimePackage.timeData.hour, recvTimePackage.timeData.minute, recvTimePackage.timeData.second);
					break;
				case sunriseTime:
					set_time(&sunrise_time, recvTimePackage.timeData.hour, recvTimePackage.timeData.minute, recvTimePackage.timeData.second);
					break;
				case currentTime:
					set_time(&current_time, recvTimePackage.timeData.hour, recvTimePackage.timeData.minute, recvTimePackage.timeData.second);
					ESP_LOGI(TAG, "New current time: %d:%d:%d", current_time.hour, current_time.minute, current_time.second);
					break;
				default:
					ESP_LOGE(TAG, "Error! Can not find correct time type");
					break;
			}
		}
		
		vTaskDelay(xDelayDuration); //repeat every 1 second
	}
	// Never gets here!
}