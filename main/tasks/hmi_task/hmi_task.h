#ifndef HMI_TASK_H
#define HMI_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/**
 * @file hmi_task.h
 * @brief Header file for the Human-Machine Interface (HMI) task.
 *
 * This file contains the declaration of the HMI task, which is responsible
 * for managing user interactions and updating the setpoint and control mode.
 * The task communicates with other tasks via FreeRTOS queues and event groups.
 *
 * @author Sondre
 * @date 2025-03-24
 */

/**
 * @brief HMI task for managing user interactions.
 *
 * This task handles user input for updating the setpoint and control mode.
 * It synchronizes with other tasks using FreeRTOS queues and event groups.
 *
 * @param pvParameters Pointer to task parameters (can be NULL).
 */
void hmi_task(void *pvParameters);

#endif