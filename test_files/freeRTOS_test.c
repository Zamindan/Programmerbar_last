#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

void blinky(void *parameter)
{
    while (1)
    {
        gpio_set_level(GPIO_NUM_10, 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(GPIO_NUM_10, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void buzzer(void *parameter)
{
    while (1)
    {
        // Set buzzer to first frequency
        ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, 2000); // 2 kHz
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 4000); // 50% duty cycle
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay(pdMS_TO_TICKS(100));

        // Set buzzer to second frequency
        ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, 3000); // 3 kHz
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 4000); // 50% duty cycle
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main()
{
    ledc_timer_config_t pwm_timer = {
        .clk_cfg = LEDC_AUTO_CLK,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 4000};
    ESP_ERROR_CHECK(ledc_timer_config(&pwm_timer));

    ledc_channel_config_t pwm_config = {
        .gpio_num = 0,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .duty = 0,
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_config));


    gpio_set_direction(GPIO_NUM_10, GPIO_MODE_OUTPUT);
    xTaskCreatePinnedToCore(blinky, "Toggle LED", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(buzzer, "Buzzer Sound", 2048, NULL, 1, NULL, 1);
    xTaskCreate()
}