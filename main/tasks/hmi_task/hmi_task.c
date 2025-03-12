#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "hmi_task.h"
#include "measurement_task.h"
// #include "communication_task.h" // Uncomment this line after creating the communication task

static const char *TAG = "HMI_TASK";

QueueHandle_t mode_queue;

QueueHandle_t setpoint_queue;



void hmi_task(void *pvParameters)
{   
    float setpoint = 0.0;
    float previous_setpoint = 0.0;
    ControlMode mode = MODE_CC;
    setpoint_queue = xQueueCreate(1, sizeof(float));
    if (setpoint_queue == NULL)
    {
        ESP_LOGE(TAG, "Setpoint queue failed to create.");
    }
    else
    {
        ESP_LOGI(TAG, "Setpoint queue created.");
    }

    mode_queue = xQueueCreate(1, sizeof(ControlMode));
    if (mode_queue == NULL)
    {
        ESP_LOGE(TAG, "Mode queue failed to create.");
    }
    else
    {
        ESP_LOGI(TAG, "Mode queue created.");
    }

    while (1)
    {
        if (setpoint != previous_setpoint)
        {
            previous_setpoint = setpoint;
            xQueueOverwrite(setpoint_queue, &setpoint);
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        else if (xQueuePeek(setpoint_queue, &setpoint, pdTICKS_TO_MS(1)) == pdTRUE)
        {
            ESP_LOGI(TAG, "Received setpoint data");
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    

        if (xQueuePeek(mode_queue, &mode, pdTICKS_TO_MS(1)) == pdTRUE)
        {
            ESP_LOGI(TAG, "Received mode data");
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        
    }
}