#ifndef ADC_H
#define ADC_H

#include "esp_adc/adc_oneshot.h"

void adc_unit_init(adc_oneshot_unit_handle_t *adc_handle_name,
                   adc_unit_t adc_unit,
                   adc_ulp_mode_t ulp_mode);

void adc_channel_init(adc_oneshot_unit_handle_t adc_handle_name,
                      adc_channel_t channel, adc_atten_t atten,
                      adc_bitwidth_t bitwidth);

int adc_read(adc_oneshot_unit_handle_t adc_handle_name,
             adc_channel_t channel);
#endif