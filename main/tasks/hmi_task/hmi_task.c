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

/**
 * @file hmi_task.c
 * @brief Implementation of the Human-Machine Interface (HMI) task.
 *
 * This file contains the implementation of the HMI task, which is responsible
 * for managing user interactions and updating the setpoint and control mode.
 * The task communicates with other tasks via FreeRTOS queues and event groups.
 *
 * @note The communication task header is commented out and should be included
 *       once the communication task is implemented.
 *
 * @date 2025-05-12
 */

static const char *TAG = "HMI_TASK"; /**< Tag for logging messages from the HMI task. */

/**
 * @brief HMI task for managing user interactions.
 *
 * This task handles user input for updating the setpoint and control mode.
 * It synchronizes with other tasks using FreeRTOS queues and event groups.
 *
 * @param pvParameters Pointer to task parameters (can be NULL).
 */
void hmi_task(void *pvParameters)
{   
    float setpoint = 0.0; /**< Current setpoint value. */
    float previous_setpoint = 0.0; /**< Previous setpoint value for comparison. */
    ControlMode mode = MODE_CC; /**< Current control mode (default is Constant Current). */
    

    while (1)
    {
        // Check if the setpoint has changed
        if (setpoint != previous_setpoint)
        {
            previous_setpoint = setpoint;

            // Update the setpoint queue and signal other tasks
            xQueueOverwrite(setpoint_queue, &setpoint);
            xEventGroupSetBits(signal_event_group, CONTROL_SETPOINT_BIT);
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        // Check if another task has updated the setpoint
        else if (xEventGroupGetBits(signal_event_group) & HMI_SETPOINT_BIT)
        {
            ESP_LOGI(TAG, "Received setpoint data");

            // Clear the HMI setpoint bit and update the local setpoint
            xEventGroupClearBits(signal_event_group, HMI_SETPOINT_BIT);
            xQueuePeek(setpoint_queue, &setpoint, pdMS_TO_TICKS(1));
            previous_setpoint = setpoint;

            // Short delay to allow other tasks to process the signal
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        // No changes detected, delay to reduce CPU usage
        else
        {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        
    }
}