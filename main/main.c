#include <stdio.h>
#include "measurement_task.h"
#include "control_task.h"
#include "hmi_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "measurement_task.h"
#include "control_task.h"
#include "esp_log.h"
#include "adc.h"
#include "driver/gpio.h"
#include "i2c.h"
#include "pwm.h"
#include "driver/ledc.h"
#include "safety_task.h"
#include "globals.h"
#include "config.h"
#include "safety_task.h"
#include "wifi.h"
#include "http_server.h"



static const char *TAG = "MAIN";


void app_main()
{
    pwm_init();
    pwm_update_duty(50, PWM_CHANNEL_FAN);
}