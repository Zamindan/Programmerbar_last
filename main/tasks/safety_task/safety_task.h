#ifndef SAFETY_TASK_H
#define SAFETY_TASK_H

/**
 * @file safety_task.h
 * @brief Header file for the safety task.
 *
 * This file contains the declaration of the safety task, which is responsible
 * for monitoring safety conditions such as overvoltage, overcurrent, overtemperature,
 * and undervoltage. The task interacts with other tasks via FreeRTOS queues and
 * event groups to ensure safe operation of the system.
 *
 * @author Sondre
 * @date 2025-03-24
 */

/**
 * @brief Safety task for monitoring and enforcing safety conditions.
 *
 * This task continuously monitors measurement data and user-defined safety limits.
 * If a safety condition is violated (e.g., overvoltage, overcurrent), it triggers
 * the appropriate safety event and disables the relays to protect the system.
 *
 * @param paramater Pointer to task parameters (can be NULL).
 */
void safety_task(void *paramater);

#endif // SAFETY_TASK_H