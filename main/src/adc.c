#include <stdio.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "adc.h"

static const char* TAG = "ADC_INIT";
/**
 * @brief Initialises an ADC unit.
 */
void adc_unit_init(adc_oneshot_unit_handle_t adc_handle_name, adc_unit_t adc_unit, adc_ulp_mode_t ulp_mode)
{
    adc_oneshot_unit_init_cfg_t adc_unit_cfg = {
        .unit_id = adc_unit,
        .ulp_mode = ulp_mode,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_unit_cfg, &adc_handle_name));
    ESP_LOGI(TAG, "ADC unit initialised successfully");
}

void adc_channel_init(adc_oneshot_unit_handle_t adc_handle_name, adc_channel_t channel, adc_atten_t atten, adc_bitwidth_t bitwidth){
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = bitwidth,
        .atten = atten,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle_name, channel, &config));
    ESP_LOGI(TAG, "ADC channel initialised successfully");
}

int adc_read(adc_oneshot_unit_handle_t adc_handle_name, adc_channel_t channel, int adc_raw){
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle_name, channel, &adc_raw));
    return adc_raw;
}