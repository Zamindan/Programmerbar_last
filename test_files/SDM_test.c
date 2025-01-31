#include <stdio.h>
#include "driver/gpio.h"
#include "driver/sdm.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    sdm_channel_handle_t chan = NULL;
    sdm_config_t config = {
        .clk_src = SDM_CLK_SRC_DEFAULT,
        .sample_rate_hz = 1 * 100,
        .gpio_num = 0,
    };

    ESP_ERROR_CHECK(sdm_new_channel(&config, &chan));
    ESP_ERROR_CHECK(sdm_channel_enable(chan));
    ESP_ERROR_CHECK(sdm_channel_set_pulse_density(chan, 90));
    ESP_LOGI("SDM", "SDM running");
}