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
#include "communication_task.h"



void app_main()
{
    //xTaskCreatePinnedToCore(measurement_task, "Measurement Task", 4096, NULL, 1, NULL, 1);

    //xTaskCreatePinnedToCore(hmi_task, "HMI Task", 4096, NULL, 1, NULL, 1);
    
    //xTaskCreatePinnedToCore(control_task, "Control Task", 4096, NULL, 1, NULL, 1);

    xTaskCreatePinnedToCore(communication_task, "Communication Task", 4096, NULL, 1, NULL, 1);

}