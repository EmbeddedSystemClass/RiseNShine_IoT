/*
clock_management.c

implementation of clock management.h

*/

#include "clock_management.h"

/*
 * Helper functions
 */
int compare_time(timeFormat_t* time1, timeFormat_t* time2)
{
	if(time1->hour == time2->hour && time1->minute == time2->minute) return 1;
	else return 0;
}

void set_time(timeFormat_t* time, unsigned int hr, unsigned int min, unsigned int sec)
{
	time->hour = hr;
	time->minute = min;
	time->second = sec;
}

void increment_time(timeFormat_t* time)
{
	time->second++;
	if (time->second >= 60) 
	{
		time->second = 0;
		time->minute++;
	}

	if (time->minute >= 60)
	{
		time->minute=0;
		time->hour++;
	}

	if (time->hour >= 24) time->hour = 0;
}


