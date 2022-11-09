#include "ui_menu.h"

void put_string(oled_driver &oled, std::string_view str, uint8_t x, uint8_t y, bool item, bool invert) {
    std::array<uint8_t, 129> data;
    data[0] = 0x40;
    auto it = data.begin() + 1;
    for (uint8_t i = 0; i < x; i++)
        *it++ = invert ? 0xFF : 0x00;

    auto put_char = [&](char c) {
        auto &font = get_font6x8(c);
        for (size_t i = 0; i < font.size(); i++)
            *it++ = invert ? ~font[i] : font[i];
    };

    if (item) {
        put_char('>');
        put_char(' ');
    }

    for (char c : str)
        put_char(c);

    while (it != data.end())
        *it++ = invert ? 0xFF : 0x00;

    oled.set_pos(0, y);
    oled.i2c_transmit(data, 10);
}

void put_string_center(oled_driver &oled, std::string_view str, uint8_t y, bool invert) {
    uint8_t x = (128 - str.size() * 6) / 2;
    put_string(oled, str, x, y, false, invert);
}

void put_string_no_fill(oled_driver &oled, std::string_view str, uint8_t x, uint8_t y, bool invert) {
    oled.set_pos(x, y);

    std::array<uint8_t, 7> data;
    data[0] = 0x40;
    for (char c : str) {
        auto &font = get_font6x8(c);
        auto it = data.begin() + 1;
        for (size_t i = 0; i < font.size(); i++)
            *it++ = invert ? ~font[i] : font[i];
        oled.i2c_transmit(data);
    }
}

void put_string_no_fill_center(oled_driver &oled, std::string_view str, uint8_t y, bool invert) {
    uint8_t x = (128 - str.size() * 6) / 2;
    put_string_no_fill(oled, str, x, y, invert);
}

void ui_menu::draw_item(uint8_t index) {
    put_string(oled, menu_items[index].name, 0, index + 2, true, index == selected);
}

void ui_menu::draw() {
    oled.page_addressing_mode();
    put_string_center(oled, title, 0);
    oled.clear(1, 2);

    for (uint8_t i = 0; i < menu_items_count; i++)
        draw_item(i);

    if (menu_items_count < 6)
        oled.clear(menu_items_count + 2);
}

void ui_menu::handle_button(uint8_t button, button_event event, uint32_t tick) {
    if (event == button_event::press) {
        if (button == 0) {
            if (selected > 0) {
                selected--;
                draw_item(selected + 1);
                draw_item(selected);
            }
        } else if (button == 2) {
            if (selected < menu_items_count - 1) {
                selected++;
                draw_item(selected - 1);
                draw_item(selected);
            }
        } else if (button == 1) {
            menu_items[selected].action();
        } else if (button == 3) {
            pm.pop();
        }
    }
}

void toast_t::reset(std::string_view text, uint32_t duration) {
    this->text = text;
    this->target_tick = duration == DURATION_INFINITY ? DURATION_INFINITY : duration + HAL_GetTick();

    // Only redraw the text line.
    constexpr int FILL_WIDTH = WIDTH - 2;
    int padding_left = (FILL_WIDTH - text.size() * 6) / 2;

    std::array<uint8_t, FILL_WIDTH + 1> data;
    data[0] = 0x40;
    auto it = data.begin() + 1;
    std::fill(it, it + padding_left, 0x00);
    it += padding_left;

    for (char c : text) {
        auto &font = get_font6x8(c);
        for (size_t i = 0; i < font.size(); i++)
            *it++ = font[i];
    }

    std::fill(it, data.end(), 0x00);
    oled.set_pos(PADDING_X + 1, TEXT_LINE);
    oled.i2c_transmit(data, 10);
}

void toast_t::show(std::string_view text, uint32_t duration) {
    this->text = text;
    this->target_tick = duration == DURATION_INFINITY ? DURATION_INFINITY : duration + HAL_GetTick();
    pm.push(*this);
}

void toast_t::draw() {
    oled.page_addressing_mode();

    std::array<uint8_t, WIDTH + 1> data;
    data[0] = 0x40;
    data[1] = 0xFF;
    std::fill(data.begin() + 2, data.end() - 1, 0x01);
    data.back() = 0xFF;
    oled.set_pos(PADDING_X, PADDING_TOP);
    oled.i2c_transmit(data, 10);

    std::fill(data.begin() + 2, data.end() - 1, 0x00);
    for (int i = PADDING_TOP + 1; i < 7 - PADDING_BOTTOM; i++) {
        oled.set_pos(PADDING_X, i);
        oled.i2c_transmit(data, 10);
    }

    std::fill(data.begin() + 2, data.end() - 1, 0x80);
    oled.set_pos(PADDING_X, 7 - PADDING_BOTTOM);
    oled.i2c_transmit(data, 10);

    put_string_no_fill_center(oled, text, TEXT_LINE);
}

void toast_t::tick(uint32_t tick) {
    if (target_tick != static_cast<decltype(target_tick)>(-1) && tick > target_tick) {
        pm.pop();
    }
}
