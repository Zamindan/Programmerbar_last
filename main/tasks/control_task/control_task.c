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
 * @date 2025-05-12
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

    // PI regulator paramters for CC mode
    const float Kp_CC = 8.0;  /**< Proportional gain CC */
    const float Ki_CC = 50.0; /**< Integral gain CC */

    // PI regulator paramters for CC mode
    const float Kp_CV = 1.0; /**< Proportional gain CC */
    const float Ki_CV = 0.1; /**< Integral gain CC */

    // PI regulator paramters for CC mode
    const float Kp_CP = 1.0; /**< Proportional gain CC */
    const float Ki_CP = 0.1; /**< Integral gain CC */

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
        if ((xEventGroupGetBits(signal_event_group) & CONTROL_SETPOINT_BIT) == CONTROL_SETPOINT_BIT)
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
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        // Check if the load should be started and no safety triggers are active
        if ((((xEventGroupGetBits(signal_event_group) & START_STOP_BIT) == START_STOP_BIT) & (xEventGroupGetBits(safety_event_group) == 0)) | (((xEventGroupGetBits(signal_event_group))&RESET_BIT) == RESET_BIT))
        {

            switch (mode)
            {
            case MODE_CC: // Constant Current Mode
                          // Control logic here

                // Calculate the error
                error = setpoint - measurements.current;

                // Update the integral term
                integral_error += error * dt;

                // Anti-windup: Clamp the integral term to prevent excessive accumulation
                if (integral_error > 100.0) // Adjust this limit as needed
                {
                    integral_error = 100.0;
                }
                else if (integral_error < -100.0)
                {
                    integral_error = -100.0;
                }

                // Calculate the control output (duty cycle)
                duty_cycle = Kp_CC * error + Ki_CC * integral_error;

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
                pwm_update_duty(duty_cycle, PWM_CHANNEL_LOAD);

                ESP_LOGI(TAG, "Setpoint: %.2f A, Measured: %.2f A, Duty Cycle: %.2f%%", setpoint, measurements.current, duty_cycle);

                break;
            case MODE_CV: // Constant Voltage Mode
                          // Control logic here

                // Calculate the error
                error = setpoint - measurements.bus_voltage;

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
                duty_cycle = -Kp_CV * error - Ki_CV * integral_error;

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
                pwm_update_duty(duty_cycle, PWM_CHANNEL_LOAD);

                ESP_LOGI(TAG, "Setpoint: %.2f V, Measured: %.2f V, Duty Cycle: %.2f%%", setpoint, measurements.bus_voltage, duty_cycle);

                break;
            case MODE_CP: // Constant Power Mode
                          // Control logic here

                // Calculate the error
                error = setpoint - measurements.power;

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
                duty_cycle = Kp_CP * error + Ki_CP * integral_error;

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
                pwm_update_duty(duty_cycle, PWM_CHANNEL_LOAD);

                ESP_LOGI(TAG, "Setpoint: %.2f W, Measured: %.2f W, Duty Cycle: %.2f%%", setpoint, measurements.power, duty_cycle);

                break;
            default:
                ESP_LOGE(TAG, "Invalid mode");
                break;
            }
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            duty_cycle = 0;
            error = 0;
            integral_error = 0;
            previous_tick = xTaskGetTickCount();
            pwm_update_duty(duty_cycle, PWM_CHANNEL_LOAD);
            ESP_LOGI(TAG, "Load stopped");
        }

        if (measurements.temperature_internal > safety_data.soft_max_temperature)
        {
            setpoint -= 0.1;
        }
        else if (measurements.current > safety_data.soft_max_current)
        {
            setpoint -= 0.01;
        }
        else if (measurements.bus_voltage > safety_data.soft_max_voltage)
        {
            ESP_LOGI(TAG, "Unsure what to do with this, so just add this later");
        }

        if ((xEventGroupGetBits(signal_event_group) & RESET_BIT) == RESET_BIT)
        {
            ESP_LOGI(TAG, "Load Reset triggered");
            xEventGroupClearBits(signal_event_group, RESET_BIT);
            xEventGroupClearBits(safety_event_group, OVERVOLTAGE_BIT);
            xEventGroupClearBits(safety_event_group, OVERCURRENT_BIT);
            xEventGroupClearBits(safety_event_group, OVERTEMPERATURE_BIT);
            xEventGroupClearBits(safety_event_group, UNDERVOLTAGE_BIT);
            xEventGroupClearBits(signal_event_group, START_STOP_BIT);
            setpoint = 0;
        }

        // Handle safety triggers
        if ((xEventGroupGetBits(safety_event_group)) != 0)
        {
            duty_cycle = 0;
            pwm_update_duty(duty_cycle, PWM_CHANNEL_LOAD);
            ESP_LOGI(TAG, "SAFETY TRIGGERED, %lu", xEventGroupGetBits(safety_event_group));
            pwm_update_duty(50, PWM_CHANNEL_BUZZER);
        }


        // For fan control I need a PWM signal that is between 18 kHz and 30 kHz
        // The fan has an operating duty cycle range from 30% to 100%
        // This is a very rudementary way to implement a fan curve but its the first thing i though of, change if it doesnt work well.
        if (measurements.temperature_internal > 80)
        {
            pwm_update_duty(100, PWM_CHANNEL_FAN);
        }
        else if (measurements.temperature_internal > 70)
        {
            pwm_update_duty(80, PWM_CHANNEL_FAN);
        }
        else if (measurements.temperature_internal > 60)
        {
            pwm_update_duty(60, PWM_CHANNEL_FAN);
        }
        else if (measurements.temperature_internal > 50)
        {
            pwm_update_duty(40, PWM_CHANNEL_FAN);
        }
        else if (measurements.temperature_internal > 40)
        {
            pwm_update_duty(30, PWM_CHANNEL_FAN);
        }
        else
        {
            pwm_update_duty(0, PWM_CHANNEL_FAN);
        }
    }
}