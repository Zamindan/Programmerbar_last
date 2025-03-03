#include "pwm.h"
#include "esp_log.h"

static const char *TAG = "CONTROL";


typedef LOAD_MODE_t
{
    MODE_CC,
    MODE_CV,
    MODE_CP
} load_mode;

void control_init(void)
{
    pwm_init();
}

void control_task(void *parameter)
{
    extern LOAD_MODE_t load_mode;
    control_init();

    while (1)
    {
        switch (load_mode)
        {
        case MODE_CC:
            // CC mode
            // Set the PWM duty cycle so current matches set_current
            ESP_LOGI(TAG, "CC mode"); //Temporary debugging tag
            break;
        case MODE_CV:
            // CV mode
            // Set the PWM duty cycle so voltage matches set_voltage
            ESP_LOGI(TAG, "CV mode");
            break;

        case MODE_CP:
            // CP mode
            // Set the PWM duty cycle so power matches set_power
            ESP_LOGI(TAG, "CP mode");
            break;
        
        default:
            // Invalid mode
            ESP_LOGI(TAG, "Invalid mode");
            break;
        }
    }
}