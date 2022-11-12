#include <ui_set_value.h>
#include <fonts.h>


namespace {
void draw_disabled(oled_driver &oled, std::string_view text) {
    auto padding_left = (128 - 16 * text.size()) / 2;

    oled.addressing_range(2, 5, padding_left);

    std::array<uint8_t, 1 + sizeof(font_ter_u32b[0])> data;
    data[0] = 0x40;
    for (auto c : text) {
        auto &font = get_font_ter_u32b(c);
        std::copy(font.begin(), font.end(), data.begin() + 1);
        oled.i2c_transmit(data, 20);
    }
}
} // namespace

void value_input::draw() {
    if (!this->enabled) {
        oled.addressing_range();
        oled.clear(2, 6);
        oled.vertical_addressing_mode();
        draw_disabled(oled, this->disabled_text);
        return;
    }

    oled.addressing_range(2, 5, padding_left);
    std::array<uint8_t, 1 + sizeof(font_ter_u32b[0])> data;
    data[0] = 0x40;
    auto it = data.begin() + 1;
    auto add_char = [&](char c) {
        auto &font = get_font_ter_u32b(c);
        std::copy(font.begin(), font.end(), it);
        oled.i2c_transmit(data, 10);
    };
    int mod = 1;
    for (int i = 1; i < num_digits; i++)
        mod *= 10;

    int integer_part = num_digits - scale;
    int fractional_part = scale;
    bool leading_zero = true;
    while (integer_part) {
        char digit = '0' + (value / mod) % 10;
        integer_part--;
        mod /= 10;
        if (leading_zero) {
            if (integer_part > 0 && digit == '0')
                digit = ' ';
            else
                leading_zero = false;
        }
        add_char(digit);
    }
    if (fractional_part) {
        add_char('.');
        while (fractional_part) {
            char digit = '0' + (value / mod) % 10;
            fractional_part--;
            mod /= 10;
            add_char(digit);
        }
    }
    for (auto c : unit)
        add_char(c);
}

void value_input::handle_button(uint8_t button, button_event event, uint32_t tick) {
    if (button == 3)
        return;
    if (event == button_event::press) {
        if (!enabled) {
            enabled = true;
            this->draw();
        } else if (button == 1 && this->disabled_text.size() > 0) {
            enabled = false;
            this->draw();
        } else if (op_dir != 0) {
            op_dir = 0;  // cancel operation if multiple buttons are pressed
        } else {
            if (button == 2 && value < max_time) {
                op_dir = 1;
            } else if (button == 0 && value > 0) {
                op_dir = -1;
            }

            if (op_dir != 0) {
                value += op_dir;
                on_change(value);
                op_delay_start = value;
                op_start_tick = tick;
                this->draw();
            }
        }
    } else if (event == button_event::release) {
        if (button == 2 || button == 0) {
            op_dir = 0;
        }
    }
}

void value_input::tick(uint32_t tick) {
    if (op_dir != 0 && tick - op_start_tick > 500) {
        int32_t t = tick - op_start_tick - 500;
        int32_t next_value = op_delay_start + op_dir * t / 100;
        if (t > 2000) {
            int32_t t2 = (t - 2000) >> 2;
            next_value += op_dir * (t2 * t2 * (max_time / 100) / (1000000 >> 4));
        }
        if (next_value > max_time) {
            next_value = max_time;
            op_dir = 0;
        } else if (next_value < 0) {
            next_value = 0;
            op_dir = 0;
        }
        value = next_value;
        on_change(value);
        this->draw();
    }
}
