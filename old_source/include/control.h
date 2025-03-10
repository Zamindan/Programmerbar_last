#ifndef CONTROL_H
#define CONTROL_H

#include "pwm.h"
#include "esp_log.h"

typedef enum
{
    MODE_CC,
    MODE_CV,
    MODE_CP
} load_mode_t;

struct control_data
{
    float set_current;
    float set_voltage;
    float set_power;
};

void control_init(void);

void control_task(void *parameter);

#endif