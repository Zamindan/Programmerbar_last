#include "pwm.h"
#include "esp_log.h"

static const char *TAG = "CONTROL";

extern struct sensor_data sensor_data;
extern struct control_data control_data;

typedef enum
{
    MODE_CC,
    MODE_CV,
    MODE_CP
} load_mode_t;


struct control_data
{
    float set_current;
    float set_voltage;
    float set_power;
};

void control_init(void)
{
    pwm_init();
}

void control_task(void *parameter)
{
    extern load_mode_t load_mode;
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