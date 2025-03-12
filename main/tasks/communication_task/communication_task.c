#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "hmi_task.h"
#include "communication_task.h"
#include "measurement_task.h"

static const char *TAG = "COMMUNICATION_TASK";

void communcation_task(void *parameter)
{
    float setpoint = 0.0;
    float previous_setpoint = 0.0;
    ControlMode mode = MODE_CC;
    MeasurementData measurement;

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
    }
}