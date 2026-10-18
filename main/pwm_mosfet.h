#ifndef PWM_MOSFET_H
#define PWM_MOSFET_H

#include <stdbool.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "driver/ledc.h"

// Output Pins
#define PIN_PELTIER         GPIO_NUM_13
#define PIN_PUMP            GPIO_NUM_17
#define PIN_FAN_BLOWER      GPIO_NUM_14
#define PIN_FAN_CASE        GPIO_NUM_26
#define PIN_FAN_CIRC        GPIO_NUM_27

// PWM Configuration
#define PWM_FREQ            25000       // 25kHz
#define PWM_RES             LEDC_TIMER_10_BIT // 10-bit (0-1023)
#define PWM_MAX_DUTY        1023

/**
 * @brief Initialize the MOSFETs and PWM channels.
 */
void mosfet_init(void);

/**
 * @brief Control the Peltier module.
 */
void mosfet_set_peltier(bool on);

/**
 * @brief Control the Water Pump.
 */
void mosfet_set_pump(bool on);

/**
 * @brief Control the Circulation Fan.
 */
void mosfet_set_circ(bool on);

/**
 * @brief Set the Blower Fan duty cycle with hardware fading.
 */
void pwm_set_blower(uint32_t duty, int fade_ms);

/**
 * @brief Set the Case Fan duty cycle with hardware fading.
 */
void pwm_set_case(uint32_t duty, int fade_ms);

#endif // PWM_MOSFET_H