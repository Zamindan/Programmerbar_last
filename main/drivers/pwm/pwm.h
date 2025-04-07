#ifndef PWM_H
#define PWM_H

#include "driver/ledc.h"

/**
 * @file pwm.h
 * @brief He<der file for the PWM driver.abort
 * 
 * This file contains the function prototypes for initialising and updating
 * the PWM duty cycle using the ESP32 LEDC peripheral
 * 
 * @author Sondre Pettersen
 * @date 2025-03-21
 */

 /**
  * @brief Initialises the PWM module.
  * 
  * Configures the ESP32
  * 
  */
void pwm_init();

/**
 * @brief Updates the PWM duty cycle.
 *
 * Sets the PWM duty cycle to the specified percentage. The duty cycle is clamped
 * between 0% and 100%.
 *
 * @param duty_cycle The desired duty cycle as a percentage (0.0 to 100.0).
 * @param PWM_CHANNEL The PWM channel to update.
 */
void pwm_update_duty(float duty_cycle_percentage, ledc_channel_t PWM_CHANNEL);

#endif

