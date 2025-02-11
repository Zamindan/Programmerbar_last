#include "adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

//Define the ADC unit to be used
#define ADC_UNIT ADC_UNIT_1

//Defines the ULP mode (Ultra Low Power mode)
#define ADC_ULP ADC_ULP_MODE_DISABLE

//Defines the ADC channel to be used (GPIO 10)
#define ADC_CHANNEL ADC_CHANNEL_9

//Defines the attenuation for the ADC channel
#define ADC_ATTEN ADC_ATTEN_DB_12

//Define the bit width for the ADC channel
#define ADC_BITWIDTH ADC_BITWIDTH_DEFAULT

/**
 * @brief Main application function
 * 
 * This function initialises the ADC unit and channel,
 * then continuously reads the ADC value and logs it to the monitor.
 * 
 */
void app_main(){
    //Handle for the ADC unit
    adc_oneshot_unit_handle_t adc_handle_1;

    //Initialise the ADC unit
    adc_unit_init(&adc_handle_1, ADC_UNIT, ADC_ULP);

    //Initialise the ADC channel
    adc_channel_init(adc_handle_1, ADC_CHANNEL, ADC_ATTEN, ADC_BITWIDTH);

    //Variable to store the ADC result
    int adc_result;

    //Endless loop to continously read and log the ADC value
    while (1){
        //Read the ADC value
        adc_result = adc_read(adc_handle_1, ADC_CHANNEL);

        //Log the ADC result
        ESP_LOGI("MAIN", "ADC result: %d", adc_result);

        //10ms delay
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}