#ifndef ADC_H
#define ADC_H

#include "esp_adc/adc_oneshot.h"


void adc_unit_init(adc_oneshot_unit_handle_t adc_handle_name,
                   adc_oneshot_unit_init_cfg_t adc_config_name,
                   adc_unit_t adc_unit_name);


void adc_channel_init(adc_oneshot_unit_handle_t adc_handle_name,
                      adc_oneshot_unit_init_cfg_t adc_config_name,
                      adc_oneshot_chan_cfg_t channel_config_name,
                      adc_channel_t adc_channel,
                      adc_bitwidth_t bid_width,
                      adc_atten_t attenuation);


int adc_read(adc_oneshot_unit_handle_t adc_handle_name,
             adc_oneshot_chan_cfg_t channel_config_name);


float adc_raw_to_voltage(int adc_raw,
                         float Vref,
                         int max_adc_value);