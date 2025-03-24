#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "globals.h"
#include "config.h"

/**
 * @file communication_task.c
 * @brief Implementation of the communication task.
 *
 * This file contains the implementation of the communication task, which is responsible
 * for managing setpoint values and signaling changes to other tasks. It uses FreeRTOS
 * queues and event groups for inter-task communication and synchronization.
 *
 * @author Sondre
 * @date 2025-03-24
 */

static const char *TAG = "COMMUNICATION_TASK"; /**< Tag for logging messages from the communication task. */

/**
 * @brief Communication task for managing setpoint values.
 *
 * This task handles the following responsibilities:
 * - Monitors changes to the setpoint value and signals other tasks when it changes.
 * - Updates the local setpoint value when notified by other tasks.
 * - Uses FreeRTOS queues and event groups for inter-task communication.
 *
 * @param parameter Unused parameter (can be NULL).
 */
void communication_task(void *parameter)
{
    float setpoint = 0.0;          /**< Local variable to store the current setpoint value. */
    float previous_setpoint = 0.0; /**< Local variable to store the previous setpoint value for comparison. */
    ControlMode mode = MODE_CC;    /**< Control mode (default is Constant Current mode). */
    MeasurementData measurement;   /**< Placeholder for measurement data (not used in this task). */

    while (1)
    {
        // Logic for handling setpoint values

        // If the local variable changes, signal to other tasks that it has changed.
        if (setpoint != previous_setpoint) 
        {
            previous_setpoint = setpoint;

            // Overwrite the setpoint queue with the new setpoint value.
            xQueueOverwrite(setpoint_queue, &setpoint);

            // Signal other tasks that the setpoint has been updated.
            xEventGroupSetBits(signal_event_group, HMI_SETPOINT_BIT);
            xEventGroupSetBits(signal_event_group, CONTROL_SETPOINT_BIT);

            // Short delay to allow other tasks to process the signal.
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        // If another task signals that the setpoint has changed, update the local setpoint variable.
        else if (xEventGroupGetBits(signal_event_group) & COMMUNICATION_SETPOINT_BIT)
        {
            ESP_LOGI(TAG, "Received setpoint data");

            // Clear the communication setpoint bit to acknowledge the signal.
            xEventGroupClearBits(signal_event_group, COMMUNICATION_SETPOINT_BIT);

            // Retrieve the updated setpoint value from the queue.
            xQueuePeek(setpoint_queue, &setpoint, pdMS_TO_TICKS(1));

            // Update the previous setpoint variable to match the new value.
            previous_setpoint = setpoint;

            // Short delay to allow other tasks to process the signal.
            vTaskDelay(pdMS_TO_TICKS(1));
        }
         // If no changes are detected, delay the task to reduce CPU usage.
        else
        {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}