idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "tasks/control_task" "tasks/hmi_task" "tasks/control_task" "tasks/measurement_task" "tasks/communication_task" "tasks/safety_task" "tasks/server_task" "drivers/adc" "drivers/i2c" "drivers/pwm" "drivers/spi")
