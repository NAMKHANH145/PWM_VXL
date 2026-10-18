#include "button_driver.h"
#include "esp_timer.h"

#define BTN_DEBOUNCE_MS 50 // Giảm xuống 50ms vì logic mới đã xử lý việc giữ nút

typedef struct {
    gpio_num_t pin;
    bool is_held;           // Trạng thái giữ nút
    int64_t last_event_time; // Thời điểm thay đổi trạng thái cuối cùng
} btn_state_t;

static btn_state_t buttons[] = {
    { .pin = PIN_BTN_MODE, .is_held = false, .last_event_time = 0 },
    { .pin = PIN_BTN_UP,   .is_held = false, .last_event_time = 0 },
    { .pin = PIN_BTN_DOWN, .is_held = false, .last_event_time = 0 }
};

static btn_state_t* get_btn_state(gpio_num_t pin) {
    for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++) {
        if (buttons[i].pin == pin) return &buttons[i];
    }
    return NULL;
}

void button_init(gpio_num_t pin) {
    gpio_config_t in_conf = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&in_conf);
}

bool button_is_pressed(gpio_num_t pin) {
    btn_state_t* btn = get_btn_state(pin);
    if (!btn) return false;

    int level = gpio_get_level(pin); // 0 = Nhấn (Active Low), 1 = Nhả
    int64_t now = esp_timer_get_time() / 1000; // ms

    // Nếu thời gian từ lần thay đổi trạng thái trước đó chưa đủ lâu -> Bỏ qua (Chống rung/Nhiễu)
    if (now - btn->last_event_time < BTN_DEBOUNCE_MS) {
        return false;
    }

    if (level == 0) { 
        // Đang nhấn nút
        if (!btn->is_held) {
            // Đây là thời điểm BẮT ĐẦU nhấn (Falling Edge)
            btn->is_held = true;       // Khóa trạng thái lại
            btn->last_event_time = now; // Ghi nhớ thời gian
            return true;               // Báo nhấn thành công 1 lần duy nhất
        }
        // Nếu btn->is_held vẫn là true, nghĩa là người dùng vẫn đang giữ nút -> Không làm gì cả (return false)
    } else {
        // Đang nhả nút
        if (btn->is_held) {
            // Đây là thời điểm BẮT ĐẦU nhả (Rising Edge)
            btn->is_held = false;      // Mở khóa
            btn->last_event_time = now; // Ghi nhớ thời gian nhả để chống rung khi nhả
        }
    }

    return false;
}