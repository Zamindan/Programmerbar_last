#include <stdio.h>
#include "driver/gpio.h"
#include "driver/sdm.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/ledc.h"

void app_main(void)
{
    gpio_set_pull_mode(GPIO_NUM_10, GPIO_PULLDOWN_ONLY);

    ledc_timer_config_t pwm_timer = {
        .clk_cfg = LEDC_AUTO_CLK,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_14_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 4000};
    ESP_ERROR_CHECK(ledc_timer_config(&pwm_timer));

    ledc_channel_config_t pwm_config = {
        .gpio_num = GPIO_NUM_10,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .duty = 0,
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_config));

    while (1)
    {
        for (int i = 15000; i < 16383; i++)
        {
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i));

            ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

            if (i == 16383) {
                i = 15000;
            }
            vTaskDelay(1);
        }
    }
}
