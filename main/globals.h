#ifndef GLOBALS_H
#define GLOBALS_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

/**
 * @file globals.h
 * @brief Global variables and data structures for the programmable electrical load project.
 *
 * This file contains the declarations of global variables, event groups, queues,
 * and data structures used throughout the project. These globals are shared
 * between tasks and modules to facilitate communication and synchronization.
 *
 * @author Sondre
 * @date 2025-03-21
 */

/// @name Queues
/// Queues used for inter-task communication.
//@{
extern QueueHandle_t mode_queue;        /**< Queue for control mode updates. Declared in main.c */
extern QueueHandle_t setpoint_queue;    /**< Queue for setpoint values. Declared in main.c */
extern QueueHandle_t safety_queue;      /**< Queue for safety-related data. Declared in main.c */
extern QueueHandle_t measurement_queue; /**< Queue for processed measurement data. Declared in main.c */
//@}

/// @name Event Groups
/// Event groups used for task synchronization and signaling.
//@{
extern EventGroupHandle_t signal_event_group; /**< Event group for signaling between tasks. */
extern EventGroupHandle_t safety_event_group; /**< Event group for safety-related events. */
//@}

/**
 * @brief Data structure for user-defined safety limits.
 *
 * This structure holds the user-defined safety thresholds for voltage, current,
 * power, and temperature. These values are used to ensure the system operates
 * within safe limits.
 */
typedef struct
{
    float max_voltage_user;     /**< Maximum allowable voltage set by the user (V). */
    float min_voltage_user;     /**< Minimum allowable voltage set by the user (V). */
    float max_current_user;     /**< Maximum allowable current set by the user (A). */
    float max_power_user;       /**< Maximum allowable power set by the user (W). */
    float max_temperature_user; /**< Maximum allowable temperature set by the user (°C). */
    float soft_max_current;     /**< Soft maximum allowable current set by the user (A). */
    float soft_max_voltage;     /**< Soft maximum allowable voltage set by the user (V). */
    float soft_max_temperature; /**< Soft maximum allowable temperature set by the user (°C) */
} SafetyData;                   // The difference between soft and non soft limits is that the soft limits will trigger the system regulate the PWM to stay within the limits, the others will open relays to completelt shut off the load.

/**
 * @brief Data structure for processed measurement data.
 *
 * This structure holds the processed measurement data, including voltage,
 * current, power, and temperature. These values are updated by the measurement
 * task and shared with other tasks via the `measurement_queue`.
 */
typedef struct
{
    float bus_voltage;            /**< Measured bus voltage (V). */
    float current;                /**< Measured current (A). */
    float power;                  /**< Calculated power (W). */
    float temperature_internal;   /**< Measured internal temperature (°C). */
    float temperature_external_1; /**< Measured external temperature probe 1 (°C)*/
    float temperature_external_2; /**< Measured external temperature probe 2 (°C)*/
    float temperature_external_3; /**< Measured external temperature probe 3 (°C)*/
} MeasurementData;

/**
 * @brief Enumeration for control modes.
 *
 * This enumeration defines the available control modes for the programmable
 * electrical load. The control mode determines how the load operates.
 */
typedef enum
{
    MODE_CC, /**< Constant Current mode. */
    MODE_CV, /**< Constant Voltage mode. */
    MODE_CP  /**< Constant Power mode. */
} ControlMode;

#endif // GLOBALS_H