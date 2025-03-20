#ifndef ADC_H
#define ADC_H

#include <stdio.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

/**
 * @file adc.c
 * @brief Implementation of ADC initialization and reading functions.
 * 
 * This file contains the functions for initializing an ADC unit, configuring ADC channels,
 * and reading values from ADC channels.
 * 
 * @author Sondre Pettersen
 * @date 2025-02-11
 */

/**
 * @brief Initialises an ADC unit.
 * 
 * This function initialises an ADC unit with the configuration input in the parameters
 * 
 * @param adc_handle_name Pointer to the ADC unit handle structure.
 * @param adc_unit The ADC unit to initialise.
 * @param ulp_mode The ULP mode to use.
 */
void adc_unit_init(adc_oneshot_unit_handle_t *adc_handle_name,
                   adc_unit_t adc_unit,
                   adc_ulp_mode_t ulp_mode);

/**
 * @brief Initialises an ADC channel, refer to the datsheet to see which channel corresponds to which GPIO
 * 
 * This function configures an ADC channel with the specified settings.
 * 
 * @param adc_handle_name The handle of the ADC unit structure.
 * @param channel The ADC channel to configure
 * @param atten The attenuation to use for the channel. Decides how high voltage you can measure.
 * @param bitwidth The bit width to use for the channel.
 */
void adc_channel_init(adc_oneshot_unit_handle_t adc_handle_name,
                      adc_channel_t channel, adc_atten_t atten,
                      adc_bitwidth_t bitwidth);

/**
 * @brief Reads the value from an ADC channel.
 * 
 * This function reads the raw value from the specified ADC channel.
 * 
 * @param adc_handle_name The handle of the ADC unit.
 * @param channel The ADC channel to read from.
 * @return Returns the read ADC value.
 */
int adc_read(adc_oneshot_unit_handle_t adc_handle_name,
             adc_channel_t channel);

#endif