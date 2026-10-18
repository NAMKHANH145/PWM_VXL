#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "pwm_mosfet.h"   // All hardware control is here
#include "button_driver.h" // All input logic is here

static const char *TAG = "FULL_TEST";

void app_main(void) {
    ESP_LOGI(TAG, "Starting FULL SYSTEM IO TEST...");

    // 1. Initialize Outputs (PWM + GPIOs)
    mosfet_init();

    // 2. Initialize All 3 Buttons
    button_init(PIN_BTN_MODE);
    button_init(PIN_BTN_UP);
    button_init(PIN_BTN_DOWN);

    int test_duty = 0;
    int step = 256; // 25% step

    printf("\n============================================\n");
    printf("           ALL PINS SYNC TEST               \n");
    printf("============================================\n");
    printf("INPUTS (Press ANY button):\n");
    printf(" - GPIO 32, 33, 25\n");
    printf("--------------------------------------------\n");
    printf("OUTPUTS (Sync behavior):\n");
    printf(" - PWM Fans (14, 26): Smooth Fade (0-100%%)\n");
    printf(" - Switches (13, 17, 27): ON when PWM > 0\n");
    printf("============================================\n\n");

    while (1) {
        // Check ANY of the 3 buttons
        bool btn_pressed = button_is_pressed(PIN_BTN_MODE) || 
                           button_is_pressed(PIN_BTN_UP)   || 
                           button_is_pressed(PIN_BTN_DOWN);

        if (btn_pressed) {
            test_duty += step;
            if (test_duty > PWM_MAX_DUTY) {
                test_duty = 0;
            }

            // 1. Set PWM Channels (Smooth Fade 300ms)
            pwm_set_blower(test_duty, 300);
            pwm_set_case(test_duty, 300);

            // 2. Set Digital Channels
            bool turn_on = (test_duty > 0);
            mosfet_set_peltier(turn_on);
            mosfet_set_pump(turn_on);
            mosfet_set_circ(turn_on); // New clean API

            // Log Status
            printf(">>> ACTION TRIGGERED | Level: %3d%% | Digital: %s\n", 
                   (int)((test_duty / 1023.0) * 100), 
                   turn_on ? "ON" : "OFF");
        }

        vTaskDelay(pdMS_TO_TICKS(20)); // Fast polling
    }
}