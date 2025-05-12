#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "config.h"

/**
 * @file pwm.c
 * @brief Implementation of the PWM driver.
 *
 * This file contains the implementation of functions for initializing and updating
 * the PWM duty cycle using the ESP32 LEDC peripheral. The PWM is configured to
 * operate on a specific GPIO pin with a defined frequency and resolution.
 *
 * @date 2025-05-12
 */

static const char *TAG = "PWM"; /**< Tag for logging messages from the PWM module. */


/**
 * @brief Initializes the PWM module.
 *
 * Configures the ESP32 LEDC peripheral for PWM generation. The PWM is set up
 * with a frequency of 5 kHz, a resolution of 10 bits, and operates on GPIO 18.
 * The timer and channel configurations are defined in the constants above.
 */
void pwm_init()
{
    ledc_timer_config_t pwm_timer = {
        .clk_cfg = LEDC_AUTO_CLK,
        .speed_mode = PWM_SPEED_MODE,
        .duty_resolution = PWM_TIMER_RESOLUTION,
        .timer_num = PWM_TIMER,
        .freq_hz = PWM_FREQ};
    ESP_ERROR_CHECK(ledc_timer_config(&pwm_timer));

    ledc_channel_config_t pwm_config_load = {
        .gpio_num = PWM_GPIO_MOSFET,
        .speed_mode = PWM_SPEED_MODE,
        .channel = PWM_CHANNEL_LOAD,
        .timer_sel = PWM_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .duty = 0,
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_config_load));
    ESP_LOGI(TAG, "PWM load initialized");

    ledc_channel_config_t pwm_config_fan = {
        .gpio_num = PWM_GPIO_FAN,
        .speed_mode = PWM_SPEED_MODE,
        .channel = PWM_CHANNEL_FAN,
        .timer_sel = PWM_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .duty = 0,
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_config_fan));
    ESP_LOGI(TAG, "PWM fan initialized");

    ledc_channel_config_t pwm_config_buzzer = {
        .gpio_num = PWM_GPIO_BUZZER,
        .speed_mode = PWM_SPEED_MODE,
        .channel = PWM_CHANNEL_BUZZER,
        .timer_sel = PWM_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .duty = 0,
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_config_buzzer));
    ESP_LOGI(TAG, "PWM fan initialized");
}

/**
 * @brief Updates the PWM duty cycle.
 *
 * Sets the PWM duty cycle to the specified percentage. The duty cycle is clamped
 * between 0% and 100%. The duty cycle value is calculated based on the resolution
 * of the timer (10 bits in this case).
 *
 * @param duty_cycle_percentage The desired duty cycle as a percentage (0.0 to 100.0).
 * @param PWM_CHANNEL The PWM channel to update.
 */
void pwm_update_duty(float duty_cycle_percentage, ledc_channel_t PWM_CHANNEL)
{
    // Ensure the duty cycle percentage is within 0 to 100
    if (duty_cycle_percentage < 0) duty_cycle_percentage = 0;
    if (duty_cycle_percentage > 100) duty_cycle_percentage = 100;

    // Calculate the duty cycle value based on the resolution
    uint32_t pwm_setpoint = (uint32_t)((duty_cycle_percentage / 100.0) * ((1 << PWM_TIMER_RESOLUTION) - 1));
    ESP_ERROR_CHECK(ledc_set_duty(PWM_SPEED_MODE, PWM_CHANNEL, pwm_setpoint));
    ESP_ERROR_CHECK(ledc_update_duty(PWM_SPEED_MODE, PWM_CHANNEL));
    //ESP_LOGI(TAG, "PWM duty cycle set to %lu (%.2f%%)", pwm_setpoint, duty_cycle_percentage);
}