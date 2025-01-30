#include <stdio.h>
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

int adc_raw;
float Vcc = 3.3;
float Vin;
const int Dmax = 4095;
void app_main(void)
{
    // Initialise an ADC
    adc_oneshot_unit_handle_t adc1_test; // Make an ADC handle called adc1_test, basically just a name
    adc_oneshot_unit_init_cfg_t init_config1 = {
        // Make a confuguration for future ADC uses, says to use ADC unit 1 and Disable ULP
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_test)); // Makes a new ADC unit using the handle and config from earlier, at the same time it's error checked.

    adc_oneshot_chan_cfg_t config = {
        // Code to configure an ADC channel inside the unit created above.
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_test, ADC_CHANNEL_0, &config)); // This line configures a specific ADC channel using the configuration specified in config.
    //
    //ADC er ferdig å initialisere
    while (1)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_test, ADC_CHANNEL_0, &adc_raw));
        Vin = ((adc_raw * Vcc) / Dmax);
        ESP_LOGI("ADC_TEST", "ADC%d Channel[%d] Raw data: %d, Voltage: %f, Test for Git", ADC_UNIT_1, ADC_CHANNEL_0, adc_raw, Vin);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}