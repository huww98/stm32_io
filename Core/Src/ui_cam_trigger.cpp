#include "ui_cam_trigger.h"

void ui_cam_trigger::update_order() {
    for (size_t i = 0; i < order.size(); i++) {
        if (shutter_delay[i] == static_cast<uint16_t>(-1)) {
            order[i] = 0;
        } else {
            order[i] = 1;
            for (size_t j = 0; j < order.size(); j++) {
                if (shutter_delay[j] < shutter_delay[i])
                    order[i]++;
            }
        }
    }
}

void ui_cam_trigger::draw_title() {
    static constexpr auto title_padding_left = (128 - 6 * title_txt.size()) / 2;
    static constexpr auto title_padding_right = (128 - 6 * title_txt.size() - title_padding_left);
    std::array<uint8_t, 129> title;
    auto it = title.begin();
    *(it++) = 0x40;
    std::fill_n(it, title_padding_left, 0xff);
    it += title_padding_left;

    for (char c : title_txt) {
        auto &font = get_font(c);
        for (int i = 0; i < 6; i++)
            *(it++) = ~font[i];
    }

    std::fill_n(it, title_padding_right, 0xff);

    oled.i2c_transmit(title, 10);
}

void ui_cam_trigger::draw_cam(int pos) {
    bool selected = this->selected == pos;
    auto order = this->order[pos];

    std::array<uint8_t, 17> top = {0x40};
    std::array<uint8_t, 17> bottom = {0x40};

    auto num_at = [&](int num, int pos) {
        for (int i = 0; i < 6; i++) {
            uint8_t font = ~get_font('0' + num)[i];
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

void ui_cam_trigger::draw() {
    oled.page_addressing_mode();
    oled.set_pos(0, 0);
    draw_title();

    for (int i = 0; i < 24; i++) {
        draw_cam(i);
    }
}

void ui_cam_trigger::handle_button(uint8_t button, button_event event) {
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
            //FIXME: Just to test the hardware
            if (shutter_delay[selected] == static_cast<uint16_t>(-1)) {
                shutter_delay[selected] = 0;
            } else {
                shutter_delay[selected] = static_cast<uint16_t>(-1);
            }
            update_order();
            this->draw_cam(selected);

            std::array<uint8_t, 3> data;
            for (int i = 0; i < 3; i++) {
                for (int j = 7; j >= 0; j--) {
                    data[i] <<= 1;
                    data[i] |= shutter_delay[i * 8 + j] == static_cast<uint16_t>(-1);
                }
            }
            this->shutter_trigger.write(data);
        }
    }
}
