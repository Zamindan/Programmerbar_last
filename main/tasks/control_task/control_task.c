#include "control_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "measurement_task.h"
#include "hmi_task.h"
#include "pwm.h"
#include "safety_task.h"

static const char *TAG = "CONTROL_TASK";

void control_task(void *paramter)
{
    ControlMode mode = MODE_CC;
    MeasurementData measurements;

    float duty_cycle = 0.0;
    float setpoint = 0.0;
    pwm_init();
    while (1)
    {

        if (xEventGroupGetBits(signal_event_group) & CONTROL_SETPOINT_BIT) // If the local variable of another task changes, changes the local one.
        {
            ESP_LOGI(TAG, "Received setpoint data");
            xEventGroupClearBits(signal_event_group, CONTROL_SETPOINT_BIT);
            xQueuePeek(setpoint_queue, &setpoint, pdMS_TO_TICKS(1));
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        if (xQueuePeek(measurement_queue, &measurements, pdTICKS_TO_MS(1)) == pdTRUE)
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        if (xQueuePeek(mode_queue, &mode, pdTICKS_TO_MS(1)) == pdTRUE)
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        if ((xEventGroupGetBits(signal_event_group) && START_STOP_BIT) & (xEventGroupGetBits(hmi_safety_event_group) == 0))
        {

            switch (mode)
            {
            case MODE_CC:
                // Control logic here
                if (measurements.current < setpoint)
                {
                    ESP_LOGI(TAG, "Current is below setpoint: %f", measurements.current);
                    duty_cycle = duty_cycle + 0.1;
                    if (duty_cycle > 60)
                    {
                        duty_cycle = 60;
                    }
                    pwm_update_duty(duty_cycle);
                }
                else if (measurements.current > setpoint)
                {
                    ESP_LOGI(TAG, "Current is above setpoint: %f", measurements.current);
                    duty_cycle = duty_cycle - 0.1;
                    if (duty_cycle < 0)
                    {
                        duty_cycle = 0;
                    }
                    pwm_update_duty(duty_cycle);
                }
                else
                {
                    ESP_LOGI(TAG, "Current is at setpoint: %f", measurements.current);

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
        else{
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        if ((xEventGroupGetBits(hmi_safety_event_group) != 0))
        {
            duty_cycle = 0;
            ESP_LOGI(TAG, "SAFETY TRIGGERED");
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}