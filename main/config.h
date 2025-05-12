#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file config.h
 * @brief Configuration file for the programmable electrical load project.
 *
 * This file contains all the configuration constants, including GPIO pin assignments,
 * WiFi credentials, safety thresholds, event group bits, and PWM settings.
 * These constants are used throughout the project to ensure consistency and
 * maintainability.
 *
 *
 * @date 2025-05-12
 */

// GPIO Pins
#define PWM_GPIO_MOSFET 18        /**< GPIO pin used for PWM output for MOSFETs. */
#define PWM_GPIO_FAN 0            /**< GPIO pin used for PWM output for fan control. */
#define PWM_GPIO_BUZZER 36        /**< GPIO pin used for PWM output for buzzer. */
#define DUT_RELAY_PIN 48          /**< GPIO pin used for relay between DUT and charger. */
#define POWER_SWITCH_RELAY_PIN 45 /**< GPIO pin used for relay to physically disconnect DUT from laod. */
#define I2C_SDA_PIN 11          /**< GPIO pin used for I2C SDA. */
#define I2C_SCL_PIN 12          /**< GPIO pin used for I2C SCL. */

// WiFi Configuration
#define CONFIG_WIFI_SSID "Sondre"       /**< WiFi SSID for connecting the ESP32-S3. */
#define CONFIG_WIFI_PASSWORD "123456sp" /**< WiFi password for connecting the ESP32-S3. */

// Safety Thresholds
#define MAX_CURRENT 11.0      /**< Maximum allowable current in amperes (A). */
#define MAX_VOLTAGE 50.0      /**< Maximum allowable voltage in volts (V). */
#define MAX_TEMPERATURE 125.0 /**< Maximum allowable temperature in degrees Celsius (°C). */

// Bits for protection triggering (safety_event_group)
#define OVERVOLTAGE_BIT 1 << 0     /**< Event group bit for overvoltage protection. */
#define OVERCURRENT_BIT 1 << 1     /**< Event group bit for overcurrent protection. */
#define UNDERVOLTAGE_BIT 1 << 2    /**< Event group bit for undervoltage protection. */
#define OVERTEMPERATURE_BIT 1 << 3 /**< Event group bit for overtemperature protection. */

// Bits for signal event group
#define HMI_SETPOINT_BIT 1 << 0           /**< Event group bit for HMI setpoint updates. */
#define CONTROL_SETPOINT_BIT 1 << 2       /**< Event group bit for control setpoint updates. */
#define START_STOP_BIT 1 << 3             /**< Event group bit for start/stop control. */
#define RESET_BIT 1 << 4                  /**< Event group bit for reset control. */

// Bits for WiFi event group and WiFi-related operations
#define WIFI_SUCCESS 1 << 0 /**< Event group bit for successful WiFi connection. */
#define WIFI_FAILURE 1 << 1 /**< Event group bit for WiFi connection failure. */
#define TCP_SUCCESS 1 << 0  /**< Event group bit for successful TCP connection. */
#define TCP_FAILURE 1 << 1  /**< Event group bit for TCP connection failure. */
#define MAX_FAILURES 10     /**< Maximum number of WiFi connection retries before failure. */

// PWM Related
#define PWM_SPEED_MODE LEDC_LOW_SPEED_MODE     /**< LEDC speed mode (low speed). */
#define PWM_TIMER_RESOLUTION LEDC_TIMER_10_BIT /**< PWM timer resolution (10 bits). */
#define PWM_FREQ 30000                         /**< PWM frequency in Hz. */
#define PWM_CHANNEL_LOAD LEDC_CHANNEL_0        /**< LEDC channel used for PWM. */
#define PWM_CHANNEL_FAN LEDC_CHANNEL_1         /**< LEDC channel used for PWM. */
#define PWM_CHANNEL_BUZZER LEDC_CHANNEL_2      /**< LEDC channel used for PWM. */
#define PWM_TIMER LEDC_TIMER_0                 /**< LEDC timer used for PWM. */

// INA237 Registers
#define INA237_VBUS_REG 0x05    /**< INA237 Bus voltage register */
#define INA237_CURRENT_REG 0x07 /**< INA237 current read register */

// I2C Related
#define I2C_PORT -1                        /**< I2C port number. -1 indicates unconfigured. */
#define CLK_SRC I2C_CLK_SRC_DEFAULT        /**< I2C clock source. Uses the default clock source. */
#define GLITCH_IGNORE_COUNT 7              /**< Number of glitches to ignore on the I2C bus. */
#define INTERNAL_PULLUP 1                  /**< Enable (1) or disable (0) internal pull-up resistors. */
#define DEV_ADDR_LENGTH I2C_ADDR_BIT_LEN_7 /**< I2C device address length in bits (7-bit addressing). */

// NTC Related
#define R1_NTC_VDIV 10000   /**< Value of R1 in the voltage divider for NTC thermistor. */
#define T0_NTC 298.15       /**< Reference temperature for NTC thermistor. */
#define R0_NTC 10000        /**< Reference resistance for NTC thermistor. */
#define B_NTC_INTERNAL 3500 /**< B value for internal NTC thermistor. */
#define B_NTC_EXTERNAL 3500 /**< B value for external NTC thermistors. */

#endif