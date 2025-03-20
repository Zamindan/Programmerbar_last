#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#include "freertos/FreeRTOS.h" //
#include "freertos/task.h" //
#include "freertos/queue.h" //
#include "esp_log.h"
#include "globals.h" //
#include "config.h" //





static const char *TAG = "COMMUNICATION_TASK";



// Communication task
void communication_task(void *parameter)
{   
    float setpoint = 0.0;
    float previous_setpoint = 0.0;
    ControlMode mode = MODE_CC;
    MeasurementData measurement;

    

    while (1)
    {
        // Logic for handling setpoint values
        if (setpoint != previous_setpoint) // If the local variable changes, signal to other tasks that it has changed.
        {
            previous_setpoint = setpoint;
            xQueueOverwrite(setpoint_queue, &setpoint);
            xEventGroupSetBits(signal_event_group, HMI_SETPOINT_BIT);
            xEventGroupSetBits(signal_event_group, CONTROL_SETPOINT_BIT);
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        else if (xEventGroupGetBits(signal_event_group) & COMMUNICATION_SETPOINT_BIT) // If the local variable of another task changes, changes the local one.
        {
            ESP_LOGI(TAG, "Received setpoint data");
            xEventGroupClearBits(signal_event_group, COMMUNICATION_SETPOINT_BIT);
            xQueuePeek(setpoint_queue, &setpoint, pdMS_TO_TICKS(1));
            previous_setpoint = setpoint;
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}