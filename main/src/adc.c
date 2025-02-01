#include <stdio.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

void adc_unit_init(adc_oneshot_unit_handle_t adc_handle_name, adc_oneshot_unit_init_cfg_t adc_config_name, adc_unit_t adc_unit_name)
{
    // Initaliserer ADC unit
    adc_oneshot_unit_handle_t adc_handle_name; // Lager en ADC handle
    adc_oneshot_unit_init_cfg_t adc_config_name = {
        // Lager en konfigurasjon som denne ADC uniten kommer til å bruke
        .unit_id = adc_unit_name,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_config_name, &adc_handle_name)); // Lager ADC uniten som vi har laget navn og konfigurert over
}

void adc_channel_init(adc_oneshot_unit_handle_t adc_handle_name, adc_oneshot_unit_init_cfg_t adc_config_name, adc_oneshot_chan_cfg_t channel_config_name, adc_channel_t adc_channel, adc_bitwidth_t bid_width, adc_atten_t attenuation)
{
    //Initialiserer ADC kanal
    adc_oneshot_chan_cfg_t channel_config_name = {
        .bitwidth = bid_width,
        .atten = attenuation,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle_name, adc_channel, &adc_config_name));
}

int adc_read(adc_oneshot_unit_handle_t adc_handle_name, adc_oneshot_chan_cfg_t channel_config_name){
    int adc_raw;
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle_name, channel_config_name, &adc_raw));
    return adc_raw;
}

float adc_raw_to_voltage(int adc_raw, float Vref, int max_adc_value){
    return ((float)adc_raw * Vref) / max_adc_value;
}