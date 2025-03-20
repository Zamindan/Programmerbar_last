#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "hmi_task.h"
#include "measurement_task.h"
#include "globals.h"
#include "config.h"
// #include "communication_task.h" // Uncomment this line after creating the communication task

static const char *TAG = "HMI_TASK";




void hmi_task(void *pvParameters)
{   
    float setpoint = 0.0;
    float previous_setpoint = 0.0;
    ControlMode mode = MODE_CC;
    

    while (1)
    {
        // Logic for handling setpoint values
        if (setpoint != previous_setpoint)// If the local variable changes, signal to other tasks that it has changed.
        {
            previous_setpoint = setpoint;
            xQueueOverwrite(setpoint_queue, &setpoint);
            xEventGroupSetBits(signal_event_group, COMMUNICATION_SETPOINT_BIT);
            xEventGroupSetBits(signal_event_group, CONTROL_SETPOINT_BIT);
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        else if (xEventGroupGetBits(signal_event_group) & HMI_SETPOINT_BIT) // If the local variable of another task changes, changes the local one.
        {
            ESP_LOGI(TAG, "Received setpoint data");
            xEventGroupClearBits(signal_event_group, HMI_SETPOINT_BIT);
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