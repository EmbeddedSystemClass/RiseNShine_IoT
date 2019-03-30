/*
 * clockdata.h
 * 
 * Header files to hold the clock data of current, sunrise and sunset time.
 * This header should NOT be included in other modules.
 */

#include "clock_management.h"

#ifdef CLOCKDATA_H
 #error "clockdata.h should not be declared anywhere else"
#else // #ifdef CLOCKDATA_H
 #define CLOCKDATA_H
#endif // #ifdef CLOCKDATA_H

timeFormat_t currentTime = {
    .hour = 0,
    .minute = 0,
    .second = 0
}

timeFormat_t sunriseTime = {
    .hour = 0,
    .minute = 0,
    .second = 0
}

timeFormat_t sunsetTime = {
    .hour = 0,
    .minute = 0,
    .second = 0
}