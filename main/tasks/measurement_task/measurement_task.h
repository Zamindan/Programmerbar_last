#ifndef MEASUREMENT_TASK_H
#define MEASUREMENT_TASK_H
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/**
 * @file measurement_task.h
 * @brief Header file for the measurement task.
 *
 * This file contains the declaration of the measurement task, which is responsible
 * for reading sensor data (voltage, current, temperature) and processing it into
 * usable values. The task communicates with other tasks via FreeRTOS queues.
 *
 * @author Sondre
 * @date 2025-03-24
 */

/**
 * @brief Measurement task for reading and processing sensor data.
 *
 * This task reads raw data from the ADC and I2C sensors, processes it into
 * meaningful values (voltage, current, power, temperature), and updates the
 * `measurement_queue` for other tasks to use.
 *
 * @param pvParameters Pointer to task parameters (can be NULL).
 */
void measurement_task(void *pvParameters);


#endif