#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include <stdbool.h>
#include "driver/gpio.h"

// Button Pin Definitions
#define PIN_BTN_MODE        GPIO_NUM_32
#define PIN_BTN_UP          GPIO_NUM_33
#define PIN_BTN_DOWN        GPIO_NUM_25

/**
 * @brief Initialize a specific button pin.
 * Configures as Input with Pull-up.
 * @param pin GPIO number
 */
void button_init(gpio_num_t pin);

/**
 * @brief Check if a button is pressed with software debouncing.
 * 
 * @param pin GPIO number to check
 * @return true if button is pressed (and debounced), false otherwise.
 */
bool button_is_pressed(gpio_num_t pin);

#endif // BUTTON_DRIVER_H
