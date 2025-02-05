#include <stdio.h>
#include "driver/gpio.h"
#include "driver/sdm.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/ledc.h"

/**
 * @brief Hovedfunksjon for applikasjonen.
 * 
 * Denne funksjonen konfigurerer GPIO, PWM-timer og PWM-kanal, og kjører en uendelig løkke
 * som justerer PWM-duty cycle.
 */
void app_main(void)
{
    // Sett GPIO 10 til å bruke pull-down motstand
    gpio_set_pull_mode(GPIO_NUM_10, GPIO_PULLDOWN_ONLY);

    // Konfigurer PWM-timer
    ledc_timer_config_t pwm_timer = {
        .clk_cfg = LEDC_AUTO_CLK,           /**< Automatisk klokkevalg for PWM-timer */
        .speed_mode = LEDC_LOW_SPEED_MODE,  /**< Lav hastighetsmodus for PWM */
        .duty_resolution = LEDC_TIMER_14_BIT, /**< 14-bit oppløsning for duty cycle */
        .timer_num = LEDC_TIMER_0,          /**< Timer nummer 0 */
        .freq_hz = 4000                     /**< Frekvens på 4000 Hz */
    };
    ESP_ERROR_CHECK(ledc_timer_config(&pwm_timer));

    // Konfigurer PWM-kanal
    ledc_channel_config_t pwm_config = {
        .gpio_num = GPIO_NUM_10,            /**< GPIO nummer 10 for PWM-utgang */
        .speed_mode = LEDC_LOW_SPEED_MODE,  /**< Lav hastighetsmodus for PWM */
        .channel = LEDC_CHANNEL_0,          /**< PWM-kanal 0 */
        .timer_sel = LEDC_TIMER_0,          /**< Bruk timer 0 for denne kanalen */
        .intr_type = LEDC_INTR_DISABLE,     /**< Deaktiver avbrudd for PWM */
        .duty = 0,                          /**< Start duty cycle på 0 */
        .hpoint = 0                         /**< Høydepunkt for PWM */
    };
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_config));

    // Uendelig løkke for å justere PWM-duty cycle
    while (1)
    {
        for (int i = 15000; i < 16383; i++)
        {
            // Sett PWM-duty cycle
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i));

            // Oppdater PWM-duty cycle
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

            // Hvis duty cycle når maksimum, resett til startverdi
            if (i == 16383) {
                i = 15000;
            }

            // Vent i 1 tick (10 ms ved standard FreeRTOS-konfigurasjon)
            vTaskDelay(1);
        }
    }
}