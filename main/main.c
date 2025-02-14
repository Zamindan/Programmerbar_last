#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "measurements.h"


void app_main(void)
{
    void measurements_task(void *parameter);
    // Create a task for the measurements
    xTaskCreatePinnedToCore(measurements_task, "measurements_task", 4096, NULL, 5, NULL, 1);
}