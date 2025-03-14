#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define PWM_SPEED_MODE LEDC_LOW_SPEED_MODE
#define PWM_TIMER_RESOLUTION LEDC_TIMER_10_BIT
#define PWM_FREQ 5000
#define PWM_CHANNEL LEDC_CHANNEL_0
#define PWM_TIMER LEDC_TIMER_0
#define PWM_GPIO 18

static const char *TAG = "PWM";

void pwm_init()
{
    ledc_timer_config_t pwm_timer = {
        .clk_cfg = LEDC_AUTO_CLK,
        .speed_mode = PWM_SPEED_MODE,
        .duty_resolution = PWM_TIMER_RESOLUTION,
        .timer_num = PWM_TIMER,
        .freq_hz = PWM_FREQ};
    ESP_ERROR_CHECK(ledc_timer_config(&pwm_timer));

    ledc_channel_config_t pwm_config = {
        .gpio_num = PWM_GPIO,
        .speed_mode = PWM_SPEED_MODE,
        .channel = PWM_CHANNEL,
        .timer_sel = PWM_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .duty = 0,
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_config));
    ESP_LOGI(TAG, "PWM initialized");
}

void pwm_update_duty(float duty_cycle_percentage)
{
    // Ensure the duty cycle percentage is within 0 to 100
    if (duty_cycle_percentage < 0) duty_cycle_percentage = 0;
    if (duty_cycle_percentage > 100) duty_cycle_percentage = 100;

    // Calculate the duty cycle value based on the resolution
    uint32_t pwm_setpoint = (uint32_t)((duty_cycle_percentage / 100.0) * ((1 << PWM_TIMER_RESOLUTION) - 1));
    ESP_ERROR_CHECK(ledc_set_duty(PWM_SPEED_MODE, PWM_CHANNEL, pwm_setpoint));
    ESP_ERROR_CHECK(ledc_update_duty(PWM_SPEED_MODE, PWM_CHANNEL));
    ESP_LOGI(TAG, "PWM duty cycle set to %lu (%.2f%%)", pwm_setpoint, duty_cycle_percentage);
}