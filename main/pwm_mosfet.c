#include "pwm_mosfet.h"
#include "driver/ledc.h"
#include "esp_err.h"

// Internal Configuration
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_TIMER              LEDC_TIMER_0
#define PWM_CH_BLOWER           LEDC_CHANNEL_0
#define PWM_CH_CASE             LEDC_CHANNEL_1

void mosfet_init(void) {
    // 1. Configure GPIOs for Simple Switches (Peltier, Pump, Circ Fan)
    gpio_config_t out_conf = {
        .pin_bit_mask = (1ULL << PIN_PELTIER) | (1ULL << PIN_PUMP) | (1ULL << PIN_FAN_CIRC),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&out_conf);

    // Set default states: ALL OFF for Safety
    gpio_set_level(PIN_FAN_CIRC, 0); 
    gpio_set_level(PIN_PELTIER, 0);
    gpio_set_level(PIN_PUMP, 0);

    // 2. Configure LEDC Timer for PWM
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_MODE,
        .timer_num  = LEDC_TIMER,
        .duty_resolution = PWM_RES,
        .freq_hz = PWM_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_conf);

    // 3. Configure LEDC Channels
    // Channel 0: Blower
    ledc_channel_config_t ch_blower = {
        .gpio_num   = PIN_FAN_BLOWER,
        .speed_mode = LEDC_MODE,
        .channel    = PWM_CH_BLOWER,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER,
        .duty       = 0,
        .hpoint     = 0
    };
    ledc_channel_config(&ch_blower);

    // Channel 1: Case Fan
    ledc_channel_config_t ch_case = {
        .gpio_num   = PIN_FAN_CASE,
        .speed_mode = LEDC_MODE,
        .channel    = PWM_CH_CASE,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER,
        .duty       = 0,
        .hpoint     = 0
    };
    ledc_channel_config(&ch_case);

    // 4. Initialize Fade Service (Required for hardware fading)
    ledc_fade_func_install(0);
}

void mosfet_set_peltier(bool on) {
    gpio_set_level(PIN_PELTIER, on ? 1 : 0);
}

void mosfet_set_pump(bool on) {
    gpio_set_level(PIN_PUMP, on ? 1 : 0);
}

void mosfet_set_circ(bool on) {
    gpio_set_level(PIN_FAN_CIRC, on ? 1 : 0);
}

void pwm_set_blower(uint32_t duty, int fade_ms) {
    if (duty > PWM_MAX_DUTY) duty = PWM_MAX_DUTY;

    if (fade_ms > 0) {
        ledc_set_fade_with_time(LEDC_MODE, PWM_CH_BLOWER, duty, fade_ms);
        ledc_fade_start(LEDC_MODE, PWM_CH_BLOWER, LEDC_FADE_NO_WAIT);
    } else {
        ledc_set_duty(LEDC_MODE, PWM_CH_BLOWER, duty);
        ledc_update_duty(LEDC_MODE, PWM_CH_BLOWER);
    }
}

void pwm_set_case(uint32_t duty, int fade_ms) {
    if (duty > PWM_MAX_DUTY) duty = PWM_MAX_DUTY;

    if (fade_ms > 0) {
        ledc_set_fade_with_time(LEDC_MODE, PWM_CH_CASE, duty, fade_ms);
        ledc_fade_start(LEDC_MODE, PWM_CH_CASE, LEDC_FADE_NO_WAIT);
    } else {
        ledc_set_duty(LEDC_MODE, PWM_CH_CASE, duty);
        ledc_update_duty(LEDC_MODE, PWM_CH_CASE);
    }
}