#ifndef CONTROL_H
#define CONTROL_H

#include "pwm.h"
#include "esp_log.h"


void control_init(void);

void control_task(void *parameter);

#endif