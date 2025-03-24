#ifndef CONTROL_TASK_H
#define CONTROL_TASK_H

/**
 * @file control_task.h
 * @brief Header file for the control task.
 *
 * This file contains the declaration of the control task, which is responsible
 * for implementing the control logic for constant current (CC), constant voltage (CV),
 * and constant power (CP) modes. The task interacts with other tasks via queues
 * and event groups.
 *
 * @author Sondre
 * @date 2025-03-24
 */

/**
 * @brief Control task for managing load operation modes.
 *
 * This task implements the control logic for the programmable electrical load.
 * It monitors the setpoint, measurement data, and mode, and adjusts the PWM duty
 * cycle accordingly. The task also handles safety triggers and start/stop signals.
 *
 * @param pvParameters Pointer to task parameters (can be NULL).
 */
void control_task(void *pvParameters);

#endif // CONTROL_TASK_H