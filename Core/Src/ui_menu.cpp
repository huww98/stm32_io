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

void ui_menu::draw_item(uint8_t index) {
    put_string(oled, menu_items[index].name, 0, index + 2, true, index == selected);
}

void ui_menu::draw() {
    oled.page_addressing_mode();
    put_string_center(oled, title, 0);

    for (uint8_t i = 0; i < menu_items_count; i++)
        draw_item(i);
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
        }
    }
}
