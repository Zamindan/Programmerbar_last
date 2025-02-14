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


//Logging tag
static const char* TAG = "ADC_INIT";
/**
 * @brief Initialises an ADC unit.
 * 
 * This function initialises an ADC unit with the configuration input in the parameters
 * 
 * @param adc_handle_name Pointer to the ADC unit handle structure.
 * @param adc_unit The ADC unit to initialise.
 * @param ulp_mode The ULP mode to use.
 * 
 * @details The ADC unit handle structure should be declared in the main file like this:
 * @details adc_oneshot_unit_handle_t adc_handle;
 * @details The adc_unit should be either ADC_UNIT_1 or ADC_UNIT_2.
 * @details The ulp_mode should be either ADC_ULP_MODE_0 or ADC_ULP_MODE_1.
 * @details The function will initialise the ADC unit and store the handle in the adc_handle_name pointer.
 * @details The function will also print a message to the console if the initialisation was successful.
 * @details When you write the argument for the adc_handle_name it has to be a pointer to the adc_oneshot_unit_handle_t struct.
 */
void adc_unit_init(adc_oneshot_unit_handle_t *adc_handle_name, adc_unit_t adc_unit, adc_ulp_mode_t ulp_mode)
{
    //Configuration for the ADC unit.
    adc_oneshot_unit_init_cfg_t adc_unit_cfg = {
        .unit_id = adc_unit,
        .ulp_mode = ulp_mode,
    };
    //Initialise the ADC unit.
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_unit_cfg, adc_handle_name));
    ESP_LOGI(TAG, "ADC unit initialised successfully");
}

/**
 * @brief Initialises an ADC channel, refer to the datsheet to see which channel corresponds to which GPIO
 * 
 * This function configures an ADC channel with the specified settings.
 * 
 * @param adc_handle_name The handle of the ADC unit structure.
 * @param channel The ADC channel to configure
 * @param atten The attenuation to use for the channel. Decides how high voltage you can measure.
 * @param bitwidth The bit width to use for the channel.
 * 
 * @details The function will configure the ADC channel with the specified settings.
 * @details The function will print a message to the console if the configuration was successful.
 */
void adc_channel_init(adc_oneshot_unit_handle_t adc_handle_name, adc_channel_t channel, adc_atten_t atten, adc_bitwidth_t bitwidth){
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = bitwidth,
        .atten = atten,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle_name, channel, &config));
    ESP_LOGI(TAG, "ADC channel initialised successfully");
}

/**
 * @brief Reads the value from an ADC channel.
 * 
 * This function reads the raw value from the specified ADC channel.
 * 
 * @param adc_handle_name The handle of the ADC unit.
 * @param channel The ADC channel to read from.
 * @return Returns the read ADC value.
 * 
 * @details The function will read the raw value from the specified ADC channel.
 * @details The function will return the raw ADC value.
 * @details The function will print an error message to the console if the read was unsuccessful.
 * @details The function will print the read value to the console if the read was successful.
 * 
 */
int adc_read(adc_oneshot_unit_handle_t adc_handle_name, adc_channel_t channel){
    int adc_raw;
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle_name, channel, &adc_raw));
    return adc_raw;
}


/**
 * @brief Task for reading ADC values.
 * 
 * This task reads the value from an ADC channel and prints it to the console.
 * 
 * @param parameter The parameters for the task.
 * 
 * @details The parameter should be a pointer to an adc_task_params_t struct.
 * @details The struct should contain the handle of the ADC unit and the channel to read from.
 * @details The struct should look like this:
 * @details adc_task_params_t params = {
 * @details     .adc_handle = adc_handle,
 * @details     .channel = channel,
 * @details };
 */
void adc_task(void *parameter)
{
 
}