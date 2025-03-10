#include <stdio.h>
#include "measurement_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"


// Handle for the ADC unit
adc_oneshot_unit_handle_t adc_handle_1;

// Handle for the I2C bus
i2c_master_bus_handle_t bus_handle_name;

// Handle for the I2C device
i2c_master_dev_handle_t ina_handle;

void app_main()
{
    

}