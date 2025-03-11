#include "control_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "measurement_task.h"
#include "hmi_task.h"
#include "pwm.h"

static const char *TAG = "CONTROL_TASK";


void control_task(void *paramter)
{
    MeasurementData measurements;
    float duty_cycle = 0.0;
    if (measurement_queue == NULL)
    {
        ESP_LOGE(TAG, "Measurement queue not found.");
    }
    while (1)
    {
        if (xQueuePeek(measurement_queue, &measurements, pdTICKS_TO_MS(100)) == pdTRUE)
            {
                ESP_LOGI(TAG, "Received measurement data");
                vTaskDelay(pdMS_TO_TICKS(1));
            }

        if (xQueuePeek(mode_queue, &mode, pdTICKS_TO_MS(100)) == pdTRUE)
            {
                ESP_LOGI(TAG, "Received mode data");
                vTaskDelay(pdMS_TO_TICKS(1));
            }
        switch (mode)
        {
        case MODE_CC:
            ESP_LOGI(TAG, "Constant Current Mode");
            // Control logic here
            if (measurements.current < setpoint)
            {
                ESP_LOGI(TAG, "Current is below setpoint");

                pwm_update_duty(duty_cycle + 0.01);
            }
            else if (measurements.current > setpoint)
            {
                ESP_LOGI(TAG, "Current is above setpoint");

                pwm_update_duty(duty_cycle - 0.01);
            }
            else
            {
                ESP_LOGI(TAG, "Current is at setpoint");

                vTaskDelay(pdMS_TO_TICKS(100));
            }
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