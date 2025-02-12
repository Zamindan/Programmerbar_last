#include <stdio.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"


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
 */
int adc_read(adc_oneshot_unit_handle_t adc_handle_name, adc_channel_t channel){
    int adc_raw;
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle_name, channel, &adc_raw));
    return adc_raw;
}