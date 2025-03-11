#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "hmi_task.h"
// #include "communication_task.h" // Uncomment this line after creating the communication task

static const char *TAG = "HMI_TASK";

QueueHandle_t mode_queue;
ControlMode mode = MODE_CC;

float setpoint = 0;

void hmi_task(void *pvParameters)
{   

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
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}