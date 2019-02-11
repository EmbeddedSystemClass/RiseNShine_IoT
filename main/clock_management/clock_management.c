/*
clock_management.c

implementation of clock management.h

*/

#include "clock_management.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"


/*
 * Task to manage the system clocks and timings. 
 */
void vTaskClockSystem(void *pvParameters)
{

	//initialization
	const TickType_t xDelayDuration = 1000 / portTICK_PERIOD_MS;
	static time_t current_time;
	static time_t sunset_time;
	static time_t sunrise_time;

	set_time(&sunset_time, 18,0,0);
	set_time(&sunrise_time, 6, 0, 0);

	//start task
	for(;;)
	{
		increment_time(&current_time);

		if(current_time->second == 0)
		{
			if(compare_time(&current_time, &sunrise_time) == 1)
			{
				//send command to open blinds
			}
		}
		
		if(current_time->second == 30) 
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

}

/*
 * Helper functions
 */
static int compare_time(time_t* time1, time_t* time2)
{
	if(time1->hour == time2->hour || time1->minute == time2->minute) return 1;
	else return 0;
}

static void set_time(time_t* time, unsigned int hr, unsigned int min, unsigned int sec)
{
	time->hour = hr;
	time->minute = min;
	time->second = sec;
}

static void increment_time(time_t* time)
{
	time->second++;
	if (time->second => 60) 
	{
		time->second = 0;
		time->minute++;
	}

	if (time->minute => 60)
	{
		time->minute=0;
		time->hour++;
	}

	if (time->hour => 24) time->hour = 0;
}