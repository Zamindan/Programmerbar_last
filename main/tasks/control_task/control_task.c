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
#include "globals.h"
#include "config.h"

/**
 * @file control_task.c
 * @brief Implementation of the control task.
 *
 * This file contains the implementation of the control task, which is responsible
 * for managing the operation of the programmable electrical load. The task adjusts
 * the PWM duty cycle based on the selected mode (CC, CV, CP), setpoint, and measurement
 * data. It also handles safety triggers and start/stop signals.
 *
 * @author Sondre
 * @date 2025-03-24
 */

static const char *TAG = "CONTROL_TASK"; /**< Tag for logging messages from the control task. */

/**
 * @brief Control task for managing load operation modes.
 *
 * This task implements the control logic for the programmable electrical load.
 * It monitors the setpoint, measurement data, and mode, and adjusts the PWM duty
 * cycle accordingly. The task also handles safety triggers and start/stop signals.
 *
 * @param parameter Pointer to task parameters (can be NULL).
 */
void control_task(void *paramter)
{
    ControlMode mode = MODE_CC;   /**< Current control mode (default is Constant Current). */
    MeasurementData measurements; /**< Struct to hold the latest measurement data. */

    SafetyData safety_data = {/**< Struct to hold the soft safety limits */
                              .soft_max_current = 0,
                              .soft_max_voltage = 0,
                              .soft_max_temperature = 0};

    float duty_cycle = 0.0; /**< Current PWM duty cycle (0% to 100%). */
    float setpoint = 0.0;   /**< Current setpoint value. */

    // PI regulator paramters
    const float Kp = 1.0; /**< Proportional gain. Adjust based on system response. */
    const float Ki = 0.1; /**< Integral gain. Adjust based on system response. */

    float error = 0.0;          /**< Current error (setpoint - measured current). */
    float integral_error = 0.0; /**< Accumulated integral error. */

    TickType_t previous_tick = xTaskGetTickCount(); /**< Previous tick value for time calculations. */
    TickType_t current_tick;                        /**< Current tick value for time calculations. */
    float dt = 0;                                   /**< Time step in seconds. */

    pwm_init(); /**< Initialize the PWM module. */

    while (1)
    {
        // Get the current tick count to calculate dt
        current_tick = xTaskGetTickCount();
        dt = (float)(current_tick - previous_tick) / (float)configTICK_RATE_HZ;
        previous_tick = current_tick;

        // Check if the setpoint has been updated
        if (xEventGroupGetBits(signal_event_group) & CONTROL_SETPOINT_BIT)
        {
            ESP_LOGI(TAG, "Received setpoint data");
            xEventGroupClearBits(signal_event_group, CONTROL_SETPOINT_BIT);
            xQueuePeek(setpoint_queue, &setpoint, pdMS_TO_TICKS(1));
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        // Retrieve the latest measurement data
        if (xQueuePeek(measurement_queue, &measurements, pdTICKS_TO_MS(1)) == pdTRUE)
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        // Retrieve the current mode
        if (xQueuePeek(mode_queue, &mode, pdTICKS_TO_MS(1)) == pdTRUE)
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        // Retrieve the soft safety limits
        if (xQueuePeek(safety_queue, &safety_data, pdTICKS_TO_MS(1)) == pdTRUE)
        {
            vTaskDelay(pdMS_TO_TICKS(1))
        }

        // Check if the load should be started and no safety triggers are active
        if ((xEventGroupGetBits(signal_event_group) && START_STOP_BIT) & (xEventGroupGetBits(safety_event_group) == 0))
        {

            switch (mode)
            {
            case MODE_CC: // Constant Current Mode
                // Control logic here
                if (measurements.current < setpoint)
                {
                    if ((measurements.current < safety_data.soft_max_current) & (measurements.temperature_internal < safety_data.soft_max_temperature))
                    {

                        // Calculate the error
                        error = setpoint - measurements.current;

                        // Update the integral term
                        integral_error += error * dt;

                        // Anti-windup: Clamp the integral term to prevent excessive accumulation
                        if (integral_error > 10.0) // Adjust this limit as needed
                        {
                            integral_error = 10.0;
                        }
                        else if (integral_error < -10.0)
                        {
                            integral_error = -10.0;
                        }

                        // Calculate the control output (duty cycle)
                        duty_cycle = Kp * error + Ki * integral_error;
                    }
                    // Enforce soft current and temperature limit
                    else if ((measurements.current > safety_data.soft_max_current) | (measurements.temperature_internal > safety_data.soft_max_temperature))
                    {
                        ESP_LOGW(TAG, "Soft current limit exceeded: %.2f A (Limit: %.2f A)", measurements.current, safety_data.soft_max_current);
                        duty_cycle -= 1.0; // Reduce duty cycle to bring current within limit
                    }

                    // Clamp the duty cycle to valid range (0% to 100%)
                    if (duty_cycle > 100.0)
                    {
                        duty_cycle = 100.0;
                    }
                    else if (duty_cycle < 0.0)
                    {
                        duty_cycle = 0.0;
                    }

                    // Update the PWM duty cycle
                    pwm_update_duty(duty_cycle);

                    ESP_LOGI(TAG, "Setpoint: %.2f A, Measured: %.2f A, Duty Cycle: %.2f%%", setpoint, measurements.current, duty_cycle);
                }
                else
                {
                    vTaskDelay(pdMS_TO_TICKS(1));
                }
                break;
            case MODE_CV: // Constant Voltage Mode
                ESP_LOGI(TAG, "Constant Voltage Mode not yet implemented");
                // Control logic here

                break;
            case MODE_CP: // Constant Power Mode
                ESP_LOGI(TAG, "Constant Power Mode not yet implemented");
                // Control logic here

                break;
            default:
                ESP_LOGE(TAG, "Invalid mode");
                break;
            }
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        // Handle safety triggers
        if ((xEventGroupGetBits(safety_event_group)) != 0)
        {
            duty_cycle = 0;
            ESP_LOGI(TAG, "SAFETY TRIGGERED");
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}