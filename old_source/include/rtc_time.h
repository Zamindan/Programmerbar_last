#ifndef RTC_TIME_H
#define RTC_TIME_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Function to start the RTC task
void rtc_task(void *parameter);

#endif // RTC_TIME_H