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




void app_main()
{
    xTaskCreatePinnedToCore(measurement_task, "Measurement Task", 2048, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(hmi_task, "HMI Task", 2048, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(control_task, "Control Task", 2048, NULL, 1, NULL, 1);

}