#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <stdio.h>
#include "i2c.h"
#include "adc.h"
#include "freertos/freeRTOS.h"
#include "freertos/task.h"


struct measurement_data
{
    float current;
    float voltage;
    float power;
    float temperature;
};

/**
 * @file measurements.c
 * @brief Functions for reading I2C data and ADC data. Which will then be converted to current, voltage, power and temperature
 *
 * Datasheet for ina237: https://www.ti.com/lit/ds/symlink/ina237.pdf
 *
 * @author Sondre Pettersen
 * @date 2025-02-14
 */


 /**
 * @brief Function for configuring the INA237.
 *
 * @param dev_handle_name Handle to the I2C device.
 *
 * @details This function configures the INA237 with the desired settings.
 */
 void ina_config(i2c_master_dev_handle_t dev_handle_name);

 /**
 * @brief Function for reading the current from the INA237.
 *
 * @param dev_handle_name Handle to the I2C device.
 * @return float Returns the current read from the INA237.
 *
 * @details This function reads the current from the INA237 and returns it.
 */
float read_current(i2c_master_dev_handle_t dev_handle_name);

/**
 * @brief Function for reading the voltage from the INA237.
 *
 * @param dev_handle_name Handle to the I2C device.
 * @return float Returns the voltage read from the INA237.
 *
 * @details This function reads the voltage from the INA237 and returns it.
 */
float read_voltage(i2c_master_dev_handle_t dev_handle_name);

/**
 * @brief Function for reading the power from the INA237.
 *
 * @param dev_handle_name Handle to the I2C device.
 * @return float Returns the power calculted from voltage and current readings from INA237.
 *
 * @details This function reads the current and voltage from the INA237 and returns the calculated power
 */
float read_power(i2c_master_dev_handle_t dev_handle_name);

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
float read_temperature(adc_oneshot_unit_handle_t adc_handle, adc_channel_t adc_channel);

#endif