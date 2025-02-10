#include "adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ADC_UNIT ADC_UNIT_1
#define ADC_ULP ADC_ULP_MODE_DISABLE
#define ADC_CHANNEL ADC_CHANNEL_0
#define ADC_ATTEN ADC_ATTEN_DB_12
#define ADC_BITWIDTH ADC_BITWIDTH_DEFAULT

void app_main(){
    adc_oneshot_unit_handle_t adc_handle_1;
    adc_unit_init(&adc_handle_1, ADC_UNIT, ADC_ULP);
    adc_channel_init(adc_handle_1, ADC_CHANNEL, ADC_ATTEN, ADC_BITWIDTH);

    int adc_result = 0;
    while (1){
        adc_read(adc_handle_1, ADC_CHANNEL, adc_result);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}