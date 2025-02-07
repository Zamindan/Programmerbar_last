/**
 * @file main.c
 * @brief Hovedfil for applikasjonen.
 */

 #include <stdio.h>
 #include "driver/gpio.h"
 #include "esp_log.h"
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 #include "driver/ledc.h"
 
 /**
  * @brief Hovedfunksjon for applikasjonen.
  * 
  * Konfigurerer GPIO og PWM, og kjører en uendelig løkke for å justere PWM-duty cycle.
  */
 void app_main(void)
 {
     // Konfigurer GPIO 10 med pull-down
     gpio_set_pull_mode(GPIO_NUM_10, GPIO_PULLDOWN_ONLY);
 
     /**
      * @brief PWM-timer konfigurasjon (4000 Hz, 14-bit oppløsning)
      */
     ledc_timer_config_t pwm_timer = {
         .clk_cfg = LEDC_AUTO_CLK,
         .speed_mode = LEDC_LOW_SPEED_MODE,
         .duty_resolution = LEDC_TIMER_14_BIT,
         .timer_num = LEDC_TIMER_0,
         .freq_hz = 4000
     };
     ESP_ERROR_CHECK(ledc_timer_config(&pwm_timer));
     
     /**
      * @brief PWM-kanal konfigurasjon for GPIO 10
      */
     ledc_channel_config_t pwm_config = {
         .gpio_num = GPIO_NUM_10,
         .speed_mode = LEDC_LOW_SPEED_MODE,
         .channel = LEDC_CHANNEL_0,
         .timer_sel = LEDC_TIMER_0,
         .intr_type = LEDC_INTR_DISABLE,
         .duty = 0,
         .hpoint = 0
     };
     ESP_ERROR_CHECK(ledc_channel_config(&pwm_config));
 
     while (1) {
         // Øk PWM-duty cycle sakte fra 15000 til 16383
         for (int i = 15000; i < 16383; i++) {
             ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i));
             ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
             vTaskDelay(1);
         }
     }
 }