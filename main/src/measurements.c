#include <stdio.h>
#include "i2c.h"
#include "adc.h"
#include "freertos/freeRTOS.h"
#include "freertos/task.h"

/**
 * @file measurements.c
 * @brief Functions for reading I2C data and ADC data. Which will then be converted to current, voltage, power and temperature
 *
 * Datasheet for ina237: https://www.ti.com/lit/ds/symlink/ina237.pdf
 *
 * @author Sondre Pettersen
 * @date 2025-02-14
 */

#define INA237_SHUNT_V_REG 0x04
#define INA237_VBUS_REG 0x05
#define INA237_CURRENT_REG 0x07

static float current = 0;
static float voltage = 0;
static float power = 0;
static float temperature = 0;

/**
 * @brief Function for configuring the INA237.
 *
 * @param dev_handle_name Handle to the I2C device.
 *
 * @details This function configures the INA237 with the desired settings.
 */
void ina_config(i2c_master_dev_handle_t dev_handle_name)
{
    // Set the CONFIG register of the INA237
    i2c_write(dev_handle_name, 0x00, 0b0000000000000000);

    // Set the ADC configuration of the INA237
    i2c_write(dev_handle_name, 0x01, 0b1011000000000010);

    // Set the shunt calibration of the INA237
    i2c_write(dev_handle_name, 0x02, 0b0000100111000100); // Calculations from page 29: https://www.ti.com/lit/ds/symlink/ina237.pdf (Rshunt = 0.01 ohm, Current_LSB = 10/2^15, SHUNT_CAL = 2500)
}

/**
 * @brief Function for reading the current from the INA237.
 *
 * @param dev_handle_name Handle to the I2C device.
 * @return float Returns the current read from the INA237.
 *
 * @details This function reads the current from the INA237 and returns it.
 */
float read_current(i2c_master_dev_handle_t dev_handle_name)
{
    // Read the current from the INA237
    //uint16_t raw_shunt_voltage = i2c_read(dev_handle_name, INA237_SHUNT_V_REG);
    uint16_t raw_current = i2c_read(dev_handle_name, INA237_CURRENT_REG);

    // Convert the raw current data to actual current value
    current = (float) raw_current * (10.0 / 32768.0);


    // Conversion factor: 5uV/LSB
    //current = (float) raw_shunt_voltage * 5 / (1000000 * 0.01); // Convert to amperes (A)
    return current;
}

/**
 * @brief Function for reading the voltage from the INA237.
 *
 * @param dev_handle_name Handle to the I2C device.
 * @return float Returns the voltage read from the INA237.
 *
 * @details This function reads the voltage from the INA237 and returns it.
 */
float read_voltage(i2c_master_dev_handle_t dev_handle_name)
{
    // Read the voltage from the INA237
    uint16_t raw_voltage = i2c_read(dev_handle_name, INA237_VBUS_REG);

    // Convert the raw voltage data to actualt voltage value
    // Conversion factor: 3.125mV/LSB
    voltage = (float) raw_voltage * 3.125 / 1000.0; // Convert to volts (V)
    return voltage;
}

/**
 * @brief Function for reading the power from the INA237.
 *
 * @param dev_handle_name Handle to the I2C device.
 * @return float Returns the power calculted from voltage and current readings from INA237.
 *
 * @details This function reads the current and voltage from the INA237 and returns the calculated power
 */
float read_power(i2c_master_dev_handle_t dev_handle_name)
{
    // Read the power from the INA237
    power = read_voltage(dev_handle_name) * read_current(dev_handle_name);
    return power;
}

/**
 * @brief Function for reading the temperature from the ADC.
 *
 * @param adc_pin The GPIO pin the ADC is connected to.
 * @param adc_handle The handle to the ADC unit.
 * @param adc_channel The ADC channel to read from.
 * @return float Returns the temperature read from the ADC.
 *
 * @details This function reads the temperature from the ADC input, turns it into temperature and returns it.
 */
float read_temperature(adc_oneshot_unit_handle_t adc_handle, adc_channel_t adc_channel)
{
    // Read the temperature from the ADC
    temperature = adc_read(adc_handle, adc_channel);
    return temperature;
}

void measurements_task(void *parameter)
{
    // Handle for the ADC unit
    adc_oneshot_unit_handle_t adc_handle_1;

    // Initialise the ADC unit
    adc_unit_init(&adc_handle_1, ADC_UNIT_1, ADC_ULP_MODE_DISABLE);

    // Initialise the ADC channel
    adc_channel_init(adc_handle_1, ADC_CHANNEL_0, ADC_ATTEN_DB_12, ADC_BITWIDTH_12);

    // Handle for the I2C bus
    i2c_master_bus_handle_t bus_handle_name;

    // Initialise the I2C bus
    i2c_init(&bus_handle_name, GPIO_NUM_11, GPIO_NUM_12);

    // Handle for the I2C device
    i2c_master_dev_handle_t ina_handle;

    // Add the I2C device to the bus
    i2c_add_device(bus_handle_name, &ina_handle, (uint16_t) 0b1000000, 100000);
    // Configure the INA237
    ina_config(ina_handle);
    vTaskDelay(pdMS_TO_TICKS(100));
    while (1)
    {
        // Read the current, voltage, power and temperature
        read_current(ina_handle);
        vTaskDelay(pdMS_TO_TICKS(100));
        read_voltage(ina_handle);
        vTaskDelay(pdMS_TO_TICKS(100));
        read_power(ina_handle);
        vTaskDelay(pdMS_TO_TICKS(100));
        read_temperature(adc_handle_1, ADC_CHANNEL_0);

        ESP_LOGI("MEASUREMENTS", "Current: %f Voltage: %f Power: %f Temperature: %f", current, voltage, power, temperature);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
