#include <stdio.h>
#include "driver/gpio.h"
#include "driver/sdm.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"


int adc_raw;
float Vcc = 3.3;
float Vin;
const int Dmax = 4095;
float SDM_VOLTAGE;
float SDM_DENSITY_FLOAT;
int SDM_DENSITY;
const int SDM_MAX = 255;
const int SDM_MIN = 128;
float ratio;
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

    sdm_channel_handle_t chan = NULL;
    sdm_config_t config_sdm = {
        .clk_src = SDM_CLK_SRC_DEFAULT,
        .sample_rate_hz = 1 * 400000,
        .gpio_num = 0,
    };

    ESP_ERROR_CHECK(sdm_new_channel(&config_sdm, &chan));
    ESP_ERROR_CHECK(sdm_channel_enable(chan));
    ESP_LOGI("SDM", "SDM running");
    while (1)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_test, ADC_CHANNEL_0, &adc_raw));
        Vin = ((adc_raw * Vcc) / Dmax);
        ratio = (float)(adc_raw/Dmax);
        SDM_DENSITY_FLOAT = (ratio * (float)SDM_MAX - (float)SDM_MIN);
        SDM_DENSITY = (int)SDM_DENSITY_FLOAT;
        SDM_VOLTAGE = ((Vin/SDM_MAX)*(SDM_DENSITY+256) + Vin/2);
        ESP_ERROR_CHECK(sdm_channel_set_pulse_density(chan, SDM_DENSITY));
        ESP_LOGI("ADC_TEST", "ADC%d Channel[%d] Raw data: %d, Voltage: %f, VoutSDM: %f, SDM Density: %d", ADC_UNIT_1, ADC_CHANNEL_0, adc_raw, Vin, SDM_VOLTAGE, SDM_DENSITY);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
