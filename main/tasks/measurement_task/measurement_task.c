#include "measurement_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "adc.h"
#include "i2c.h"
#include "measurement_task.h"

static const char *TAG = "MEASUREMENT_TASK";
#define INA237_SHUNT_V_REG 0x04
#define INA237_VBUS_REG 0x05
#define INA237_CURRENT_REG 0x07

// Handle for the ADC unit
adc_oneshot_unit_handle_t adc_handle_1;

// Handle for the I2C bus
i2c_master_bus_handle_t bus_handle_name;

// Handle for the I2C device
i2c_master_dev_handle_t ina_handle;

void measurement_intitialize()
{
    // Initialise the ADC unit
    adc_unit_init(&adc_handle_1, ADC_UNIT_1, ADC_ULP_MODE_DISABLE);

    // Initialise the ADC channel
    adc_channel_init(adc_handle_1, ADC_CHANNEL_0, ADC_ATTEN_DB_12, ADC_BITWIDTH_12);

    // Handle for the I2C device
    i2c_init(&bus_handle_name, GPIO_NUM_11, GPIO_NUM_12);

    // Add the I2C device to the bus
    i2c_add_device(bus_handle_name, &ina_handle, (uint16_t)0b1000000, 100000);

    // Set the CONFIG register of the INA237
    i2c_write(ina_handle, 0x00, 0b0000000000000000);

    // Set the ADC configuration of the INA237
    i2c_write(ina_handle, 0x01, 0b1011000000000000);

    // Set the shunt calibration of the INA237
    i2c_write(ina_handle, 0x02, 0b0000100111000100); // Calculations from page 29: https://www.ti.com/lit/ds/symlink/ina237.pdf (Rshunt = 0.01 ohm, Current_LSB = 10/2^15, SHUNT_CAL = 2500)
}

void measurement_task(void *paramter)
{
    measurement_intitialize();
    MeasurementData measurements;
    

    while (1)
    {
        // Read raw sensors
        float raw_voltage = i2c_read(ina_handle, INA237_VBUS_REG);
        float raw_current = i2c_read(ina_handle, INA237_CURRENT_REG);
        uint16_t raw_temp = adc_read(adc_handle_1, ADC_CHANNEL_0);

        // Convert raw values into usable values:
        // Convert the raw voltage data to actualt voltage value
        // Conversion factor: 3.125mV/LSB
        measurements.bus_voltage = (float)raw_voltage * 3.125 / 1000.0; // Convert to volts (V)

        // Convert the raw current data to actual current value
        measurements.current = (float)raw_current * (8.0 / 32768.0);

        // Calculate power
        measurements.power = measurements.bus_voltage * measurements.current;

        // Calculate temperature
        measurements.temperature = (float)raw_temp; // Not yet implemented

        xQueueOverwrite(measurement_queue, &measurements);
        //ESP_LOGI(TAG, "raw_current from INA= %f", raw_current);
        // Maintain 1 kHz sampling rate
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
