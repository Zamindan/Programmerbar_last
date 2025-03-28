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
#include "globals.h"
#include "config.h"
#include "math.h"

/**
 * @file measurement_task.c
 * @brief Implementation of the measurement task.
 *
 * This file contains the implementation of the measurement task, which is responsible
 * for reading sensor data (voltage, current, temperature) and processing it into
 * usable values. The task communicates with other tasks via FreeRTOS queues.
 *
 * The task uses the INA237 sensor for voltage and current measurements and an ADC
 * channel for temperature readings. The processed data is stored in the
 * `measurement_queue` for use by other tasks.
 *
 * @note The INA237 configuration is based on the datasheet calculations.
 *
 * @author Sondre
 * @date 2025-03-24
 */

static const char *TAG = "MEASUREMENT_TASK"; /**< Tag for logging messages from the measurement task. */

// Handles for ADC and I2C peripherals
adc_oneshot_unit_handle_t adc_handle_1; /**< Handle for the ADC unit. */
adc_oneshot_unit_handle_t adc_handle_2; /**< Handle for the ADC unit. */
i2c_master_bus_handle_t i2c_handle;     /**< Handle for the I2C bus. */
i2c_master_dev_handle_t ina_handle;     /**< Handle for the INA237 device. */

/**
 * @brief Initializes the measurement peripherals.
 *
 * This function initializes the ADC and I2C peripherals, configures the INA237
 * sensor, and sets up the necessary calibration values for accurate measurements.
 */
void measurement_intitialize()
{
    // Initialise the ADC unit
    adc_unit_init(&adc_handle_1, ADC_UNIT_1, ADC_ULP_MODE_DISABLE);
    adc_unit_init(&adc_handle_2, ADC_UNIT_2, ADC_ULP_MODE_DISABLE);

    // Initialise the ADC channel
    adc_channel_init(adc_handle_1, ADC_CHANNEL_3, ADC_ATTEN_DB_12, ADC_BITWIDTH_12); // ADC_CHANNEL_3 ADC1 is GPIO 4, for internal NTC

    adc_channel_init(adc_handle_1, ADC_CHANNEL_0, ADC_ATTEN_DB_12, ADC_BITWIDTH_12); // ADC_CHANNEL_0 ADC1 is GPIO 1, for external NTC

    adc_channel_init(adc_handle_1, ADC_CHANNEL_1, ADC_ATTEN_DB_12, ADC_BITWIDTH_12); // ADC_CHANNEL_1 ADC1 is GPIO 2, for external NTC

    adc_channel_init(adc_handle_2, ADC_CHANNEL_6, ADC_ATTEN_DB_12, ADC_BITWIDTH_12); // ADC_CHANNEL_6 ADC2 is GPIO 17, for external NTC

    // Handle for the I2C device
    i2c_init(&i2c_handle, GPIO_NUM_11, GPIO_NUM_12);

    // Add the I2C device to the bus
    i2c_add_device(i2c_handle, &ina_handle, (uint16_t)0b1000000, 100000);

    // Configure the INA237 registers
    i2c_write(ina_handle, 0x00, 0b0000000000000000); // CONFIG register
    i2c_write(ina_handle, 0x01, 0b1011000000000000); // ADC configuration
    i2c_write(ina_handle, 0x02, 0b0000100111000100); // Shunt calibration (Rshunt = 0.01 ohm, Current_LSB = 10/2^15) From page 29: https://www.ti.com/lit/ds/symlink/ina237.pdf

    ESP_LOGI(TAG, "Measurement peripherals initialized");
}

/**
 * @brief Calculate the NTC resistance from the raw ADC value.
 *
 * This function calculates the NTC resistance based on the raw ADC value, the
 * voltage divider resistor value, and the maximum ADC value.
 *
 * @param raw_adc_value The raw ADC value read from the NTC sensor.
 * @param R1 The resistance of the voltage divider resistor (R1 = 10k ohm).
 * @param adc_max_value The maximum ADC value (4095 for 12-bit ADC).
 * @return The calculated NTC resistance.
 */
float ntc_resistance_calculate(uint16_t raw_adc_value, float R1, int adc_max_value)
{
    float R = (((float)raw_adc_value / (float)adc_max_value) * R1) / (1 - ((float)raw_adc_value / (float)adc_max_value));
    return R;
}

/**
 * @brief Calculate the temperature from the NTC resistance.
 *
 * This function calculates the temperature based on the NTC resistance, the
 * reference resistance, the reference temperature, and the B value.
 *
 * @param B The B value of the NTC thermistor.
 * @param R The NTC resistance.
 * @return The calculated temperature.
 */
float R_to_T(float B, float R)
{
    float T = 1 / ((1 / T0_NTC) + (1 / B) * log(R / R0_NTC));
    return T;
}

/**
 * @brief Measurement task for reading and processing sensor data.
 *
 * This task reads raw data from the INA237 sensor (voltage and current) and the
 * ADC channel (temperature), processes the data into meaningful values, and updates
 * the `measurement_queue`. The task maintains a 1 kHz sampling rate.
 *
 * @param parameter Pointer to task parameters (can be NULL).
 */
void measurement_task(void *paramter)
{
    measurement_intitialize();
    MeasurementData measurements; /**< Struct to hold the processed measurement data. */

    // Calculate Ah
    TickType_t previous_tick = xTaskGetTickCount(); /**< Previous tick value for time calculations. */
    TickType_t current_tick;                        /**< Current tick value for time calculations. */
    float dt = 0;                                   /**< Time step in seconds. */
    float dAh = 0;                                  /**< For calculating dAh. */
    float dWh = 0;                                  /**< For caclulating dWh. */

    float R_ntc_internal = 0;
    float R_ntc_external_1 = 0;
    float R_ntc_external_2 = 0;
    float R_ntc_external_3 = 0;

    while (1)
    {
        // Read raw sensors
        float raw_voltage = i2c_read(ina_handle, INA237_VBUS_REG);
        float raw_current = i2c_read(ina_handle, INA237_CURRENT_REG);
        uint16_t raw_temp_internal = adc_read(adc_handle_1, ADC_CHANNEL_0);
        uint16_t raw_temp_external_1 = adc_read(adc_handle_1, ADC_CHANNEL_1);
        uint16_t raw_temp_external_2 = adc_read(adc_handle_1, ADC_CHANNEL_3);
        uint16_t raw_temp_external_3 = adc_read(adc_handle_2, ADC_CHANNEL_6);

        R_ntc_internal = ntc_resistance_calculate(raw_temp_internal, R1_NTC_VDIV, 4095);
        R_ntc_external_1 = ntc_resistance_calculate(raw_temp_external_1, R1_NTC_VDIV, 4095);
        R_ntc_external_2 = ntc_resistance_calculate(raw_temp_external_2, R1_NTC_VDIV, 4095);
        R_ntc_external_3 = ntc_resistance_calculate(raw_temp_external_3, R1_NTC_VDIV, 4095);

        measurements.temperature_internal = R_to_T(B_NTC_INTERNAL, R_ntc_internal);
        measurements.temperature_external_1 = R_to_T(B_NTC_EXTERNAL, R_ntc_external_1);
        measurements.temperature_external_2 = R_to_T(B_NTC_EXTERNAL, R_ntc_external_2);
        measurements.temperature_external_3 = R_to_T(B_NTC_EXTERNAL, R_ntc_external_3);

        // Convert raw values into usable values:
        // Convert the raw voltage data to actualt voltage value
        // Conversion factor: 3.125mV/LSB
        measurements.bus_voltage = (float)raw_voltage * 3.125 / 1000.0; // Convert to volts (V)

        // Convert the raw current data to actual current value
        measurements.current = (float)raw_current * (8.0 / 32768.0);

        // Calculate power
        measurements.power = measurements.bus_voltage * measurements.current;

        // Calculate Ah and Wh
        current_tick = xTaskGetTickCount();
        dt = (float)(current_tick - previous_tick) / (float)configTICK_RATE_HZ;

        dAh = measurements.current * dt / 3600;
        measurements.Ah += dAh;

        dWh = measurements.power * dt / 3600;
        measurements.Wh += dWh;

        previous_tick = current_tick;

        xQueueOverwrite(measurement_queue, &measurements);

        if (xEventGroupGetBits(signal_event_group) & RESET_BIT)
        {
            measurements.Wh = 0;
            measurements.Ah = 0;
            xEventGroupClearBits(signal_event_group, RESET_BIT);
        }
        // ESP_LOGI(TAG, "raw_current from INA= %f", raw_current);
        //  Maintain 1 kHz sampling rate
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
