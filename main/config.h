#ifndef CONFIG_H
#define CONFIG_H


// GPIO Pins
#define PWM_GPIO 18
#define ADC_GPIO 34
#define BATTERY_RELAY_PIN 48
#define POWER_SWITCH_RELAY_PIN 45

// WiFi Configuration
#define CONFIG_WIFI_SSID "Sondre"
#define CONFIG_WIFI_PASSWORD "123456sp"

// Safety Thresholds
#define MAX_CURRENT 11.0
#define MAX_VOLTAGE 50.0
#define MAX_TEMPERATURE 100.0

//Bits for protection triggering (safety_event_group)
#define OVERVOLTAGE_BIT 1 << 0
#define OVERCURRENT_BIT 1 << 1
#define UNDERVOLTAGE_BIT 1 << 2
#define OVERTEMPERATURE_BIT 1 << 3

// Bits for setpoint
#define HMI_SETPOINT_BIT 1 << 0
#define COMMUNICATION_SETPOINT_BIT 1 << 1
#define CONTROL_SETPOINT_BIT 1 << 2
#define START_STOP_BIT 1 << 3

// Bits for wifi event group and wifi related
#define WIFI_SUCCESS 1 << 0
#define WIFI_FAILURE 1 << 1
#define TCP_SUCCESS 1 << 0
#define TCP_FAILURE 1 << 1
#define MAX_FAILURES 10


#endif