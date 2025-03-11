#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define TAG "PWM"
#define PWM_SPEED_MODE LEDC_LOW_SPEED_MODE
#define PWM_TIMER_RESOLUTION LEDC_TIMER_10_BIT
#define PWM_FREQ 50000
#define PWM_CHANNEL LEDC_CHANNEL_0
#define PWM_TIMER LEDC_TIMER_0
#define PWM_GPIO 10

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
        .gpio_num = 10,
        .speed_mode = PWM_SPEED_MODE,
        .channel = PWM_CHANNEL,
        .timer_sel = PWM_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .duty = 0,
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_config));
}

void pwm_update_duty(float duty_cycle)
{
    int pwm_setpoint = (int)((float)PWM_TIMER_RESOLUTION * duty_cycle);
    ESP_ERROR_CHECK(ledc_set_duty(PWM_SPEED_MODE, PWM_CHANNEL, pwm_setpoint));
    ESP_ERROR_CHECK(ledc_update_duty(PWM_SPEED_MODE, PWM_CHANNEL));
}