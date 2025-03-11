#include "control_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "measurement_task.h"

static const char *TAG = "CONTROL_TASK";

void control_task(void *paramter)
{
    MeasurementData measurements;
    extern QueueHandle_t measurement_queue;
    if (measurement_queue = NULL)
    {
        ESP_LOGE(TAG, "Measurement queue not found.");
    }
    ControlMode mode = MODE_CC;
    while (1)
    {
        if (xQueueReceive(measurement_queue, &measurements, 0) == pdTRUE)
            {
                ESP_LOGI(TAG, "Received measurement data");
                vTaskDelay(pdMS_TO_TICKS(1));
            }
        switch (mode)
        {
        case MODE_CC:
            ESP_LOGI(TAG, "Constant Current Mode");
            // Control logic here

            break;
        case MODE_CV:
            ESP_LOGI(TAG, "Constant Voltage Mode not yet implemented");
            // Control logic here

            break;
        case MODE_CP:
            ESP_LOGI(TAG, "Constant Power Mode not yet implemented");
            // Control logic here

            break;
        default:
            ESP_LOGE(TAG, "Invalid mode");
            break;
        }
    }
}