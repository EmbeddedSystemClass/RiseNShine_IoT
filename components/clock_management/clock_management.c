/*
clock_management.c

implementation of clock management.h

*/

#include "clock_management.h"
#include "clockdata.h"

static bool compareTime(timeFormat_t* time1, timeFormat_t* time2);
{
	if(time1->hour == time2->hour && time1->minute == time2->minute) return true;
	else return false;
}

/**
 * Compares two clocks
 * 
 * @param clock1 Type of clock to compare with clcok2
 * @param clock2 Type of clock to compare with clock1
 */
bool clock_compareTime(clockType_e clock1, clockType_e clock2)
{
	timeFormat_t clk1, clk2;
	clock_getTime(clock1, &clk1);
	clock_getTime(clock2, &clk2);
	return compareTime(&clk1, &clk2);
}

/**
 * Increments the current time
 * 
 * @note care should be done that multiple function calls may
 *       lead to undesired result.
 */
void clock_incrementCurrentTime();
{
	currentTime.second++;
	if (currentTime.second >= 60) 
	{
		currentTime.second = 0;
		currentTime.minute++;
	}

	if (currentTime.minute >= 60)
	{
		currentTime.minute=0;
		currentTime.hour++;
	}

	if (currentTime.hour >= 24) currentTime.hour = 0;
}

static void setTime(timeFormat_t *const srcClock, timeFormat_t *const destClock)
{
	destClock->hour		= srcClock->hour;
	destClock->minute	= srcClock->minute;
	destClock->second	= srcClock->second;
}

/**
 * Sets the time of the specified clock
 * 
 * @param clockType desired clock to change
 * @param data pointer to the new time
 */
void clock_setTime(clockType_e clockType, timeFormat_t *const data)
{
	switch(clockType)
	{
		case CLOCK_CURRENTTIME:
			setTime(data, &currentTime);
			break;
		case CLOCK_SUNSETTIME:
			setTime(data, &sunsetTime);
			break;
		case CLOCK_SUNRISETIME:
			setTime(data, &sunriseTime);
			break;
		default:
			// assert
	}
	return;
}

/**
 * Returns the data of the speciied data
 * 
 * @param clockType clock type of the desired clock
 * @param data pointer to place the data
 * @return the data of the specified clock in timeFormat_t
 */
void clock_getTime(clockType_e clockType, timeFormat_t *const data)
{
	switch(clockType)
	{
		case CLOCK_CURRENTTIME:
			setTime(&currentTime, data);
			break;
		case CLOCK_SUNSETTIME:
			setTime(&sunsetTime, data);
			break;
		case CLOCK_SUNRISETIME:
			setTime(&sunriseTime, data);
			break;
		default:
			// assert
	}
	return;
}
