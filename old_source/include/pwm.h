#ifndef PWM_H
#define PWM_H

#include "esp_log.h"
#include "driver/ledc.h"

void pwm_init(void);

void pwm_set_duty(uint32_t duty);

#endif