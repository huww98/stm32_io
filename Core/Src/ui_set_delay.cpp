#include "ui_set_delay.h"
#include "fonts.h"
#include "ui_menu.h"

void ui_individual_delay::update_order() {
    for (size_t i = 0; i < order.size(); i++) {
        if (!timing.enabled[i]) {
            order[i] = 0;
        } else {
            order[i] = 1;
            for (size_t j = 0; j < order.size(); j++) {
                if (!timing.enabled[j])
                    continue;
                if (timing.shutter_delay[j] < timing.shutter_delay[i])
                    order[i]++;
            }
        }
    }
}

void ui_individual_delay::draw_cam(int pos) {
    bool selected = this->selected == pos;
    auto order = this->order[pos];

    std::array<uint8_t, 17> top = {0x40};
    std::array<uint8_t, 17> bottom = {0x40};

    auto num_at = [&](int num, int pos) {
        for (int i = 0; i < 6; i++) {
            uint8_t font = ~get_font6x8('0' + num)[i];
            top[pos + i] = font << 4 | 0b00001100;
            bottom[pos + i] = font >> 4 | 0b00110000;
        }
    };

    if (order > 0) {
        if (order <= 9) {
            std::fill(top.begin() + 3, top.begin() + 6, 0b11111100);
            std::fill(bottom.begin() + 3, bottom.begin() + 6, 0b00111111);
            num_at(order, 6);
            std::fill(top.begin() + 12, top.begin() + 15, 0b11111100);
            std::fill(bottom.begin() + 12, bottom.begin() + 15, 0b00111111);
        } else {
            num_at(order / 10, 3);
            num_at(order % 10, 9);
        }
    } else {
        top[3] = 0b11111100;
        std::fill(top.begin() + 4, top.begin() + 14, 0b00000100);
        top[14] = 0b11111100;
        bottom[3] = 0b00111111;
        std::fill(bottom.begin() + 4, bottom.begin() + 14, 0b00100000);
        bottom[14] = 0b00111111;
    }

    if (selected) {
        for (auto it = top.begin() + 2; it != top.end() - 1; it++)
            *it |= 1;
        for (auto it = bottom.begin() + 2; it != bottom.end() - 1; it++)
            *it |= 0x80;
        top[1] = 0xff;
        bottom[1] = 0xff;
        top[16] = 0xff;
        bottom[16] = 0xff;
    }

    uint8_t x = (pos % 8) * 16;
    uint8_t y = (pos / 8) * 2 + 2;
    oled.set_pos(x, y);
    oled.i2c_transmit(top, 10);
    oled.set_pos(x, y + 1);
    oled.i2c_transmit(bottom, 10);
}

void ui_individual_delay::draw() {
    oled.page_addressing_mode();
    put_string_center(oled, title_txt, 0, true);

    update_order();
    for (int i = 0; i < 24; i++) {
        draw_cam(i);
    }
}

void ui_individual_delay::handle_button(uint8_t button, button_event event, uint32_t tick) {
    if (event == button_event::press) {
        if (button == 0 || button == 2) {
            auto last_selected = selected;
            if (button == 2) {
                this->selected = (this->selected + 1) % 24;
            } else if (button == 0) {
                this->selected = (this->selected + 23) % 24;
            }
            this->draw_cam(last_selected);
            this->draw_cam(selected);
        } else if (button == 1) {
            set_delay_page.select(selected);
            pm.push(set_delay_page);
        } else if (button == 3) {
            pm.pop();
        }
    }
}

void ui_set_delay::draw() {
    oled.page_addressing_mode();
    put_string_center(oled, title_txt, 0, true);

    oled.clear(1);
    oled.vertical_addressing_mode();

    _time_input.draw();
}

void ui_set_delay::handle_button(uint8_t button, button_event event, uint32_t tick) {
    if (event == button_event::press) {
        if (button == 3) {
            oled.addressing_range();
            timing.dirty |= _time_input.time != timing.shutter_delay[selected] || _time_input.enabled != timing.enabled[selected];
            timing.shutter_delay[selected] = _time_input.time;
            timing.enabled[selected] = _time_input.enabled;
            pm.pop();
        } else {
            _time_input.handle_button(button, event, tick);
        }
    } else {
        _time_input.handle_button(button, event, tick);
    }
}

void ui_set_delay::tick(uint32_t tick) {
    _time_input.tick(tick);
}
