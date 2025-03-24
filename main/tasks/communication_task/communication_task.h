#ifndef COMMUNICATION_TASK_H_
#define COMMUNICATION_TASK_H_

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_http_server.h"

/**
 * @file communication_task.h
 * @brief Header file for the communication task.
 *
 * This file contains the declaration of the communication task, which is responsible
 * for managing setpoint values and signaling changes to other tasks. It provides
 * the function prototype for the task and includes necessary dependencies.
 *
 * @author Sondre
 * @date 2025-03-24
 */

/**
 * @brief Communication task for managing setpoint values.
 *
 * This task handles inter-task communication related to setpoint values. It monitors
 * changes to the setpoint, updates the local setpoint value, and signals other tasks
 * when changes occur. The task uses FreeRTOS queues and event groups for communication
 * and synchronization.
 *
 * @param pvParameters Pointer to task parameters (can be NULL).
 */
void communication_task(void *pvParameters);

#endif // COMMUNICATION_TASK_H_