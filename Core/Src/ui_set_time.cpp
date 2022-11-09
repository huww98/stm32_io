#include "ui_set_time.h"
#include "fonts.h"
#include "ui_menu.h"

void ui_individual_delay::update_order() {
    for (size_t i = 0; i < order.size(); i++) {
        if (!enabled[i]) {
            order[i] = 0;
        } else {
            order[i] = 1;
            for (size_t j = 0; j < order.size(); j++) {
                if (!enabled[j])
                    continue;
                if (shutter_delay[j] < shutter_delay[i])
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

constexpr uint32_t CONFIG_ADDR = 0x0800f800;
constexpr uint16_t CONFIG_MAGIC = 0x1234;

void ui_individual_delay::save_config() {
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef erase {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks = FLASH_BANK_1,
        .PageAddress = CONFIG_ADDR,
        .NbPages = 1,
    };
    uint32_t page_error;
    HAL_FLASHEx_Erase(&erase, &page_error);
    assert(page_error == 0xffffffff);

    auto next_config_addr = CONFIG_ADDR;
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, next_config_addr, CONFIG_MAGIC);
    next_config_addr += 2;

    for (int i = 0; i < 24; i++) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, next_config_addr, shutter_delay[i]);
        next_config_addr += 2;
    }
    uint32_t enabled_packed = 0;
    for (int i = 0; i < 24; i++) {
        enabled_packed |= (uint32_t)enabled[i] << i;
    }
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, next_config_addr, enabled_packed);
    next_config_addr += 4;
    assert(next_config_addr <= CONFIG_ADDR + 0x400);
    HAL_FLASH_Lock();
}

void ui_individual_delay::read_config() {
    if (*(uint16_t *)CONFIG_ADDR != CONFIG_MAGIC)
        return;
    auto next_config_addr = CONFIG_ADDR + 2;
    for (int i = 0; i < 24; i++) {
        shutter_delay[i] = *(uint16_t *)next_config_addr;
        next_config_addr += 2;
    }
    uint32_t enabled_packed = *(uint32_t *)next_config_addr;
    for (int i = 0; i < 24; i++) {
        enabled[i] = enabled_packed & (1 << i);
    }
}

namespace {
void draw_OFF(oled_driver &oled) {
    constexpr std::string_view OFF = "OFF";
    auto padding_left = (128 - 16 * OFF.size()) / 2;

    oled.addressing_range(2, 5, padding_left);

    std::array<uint8_t, 1 + sizeof(font_ter_u32b[0]) * 3> data;
    data[0] = 0x40;
    auto it = data.begin() + 1;
    for (auto c : OFF) {
        auto &font = get_font_ter_u32b(c);
        it = std::copy(font.begin(), font.end(), it);
    }
    oled.i2c_transmit(data, 20);
}

void draw_time(oled_driver &oled, uint16_t time) {
    constexpr std::string_view UNIT = "ms";
    constexpr int NUM_DIGITS = 5;
    constexpr int NUM_CHARS = NUM_DIGITS + 1 + UNIT.size();
    constexpr auto padding_left = (128 - 16 * NUM_CHARS) / 2;

    oled.addressing_range(2, 5, padding_left);
    std::array<uint8_t, 1 + sizeof(font_ter_u32b[0]) * NUM_CHARS> data;
    data[0] = 0x40;
    auto it = data.begin() + 1;
    auto add_char = [&](char c) {
        auto &font = get_font_ter_u32b(c);
        it = std::copy(font.begin(), font.end(), it);
    };
    int scale = 10000;
    bool leading_zero = true;
    while (scale >= 10) {
        char digit = '0' + (time / scale) % 10;
        if (leading_zero) {
            if (scale > 10 && digit == '0')
                digit = ' ';
            else
                leading_zero = false;
        }
        add_char(digit);
        scale /= 10;
    }
    add_char('.');
    add_char('0' + time % 10);
    for (auto c : UNIT)
        add_char(c);

    oled.i2c_transmit(data, 30);
}
} // namespace

void ui_set_delay::draw() {
    oled.page_addressing_mode();
    put_string_center(oled, title_txt, 0, true);

    oled.clear(1);
    oled.vertical_addressing_mode();

    if (enabled[selected])
        draw_time(oled, shutter_delay[selected]);
    else
        draw_OFF(oled);
}

void ui_set_delay::handle_button(uint8_t button, button_event event, uint32_t tick) {
    if (event == button_event::press) {
        if (button == 1) {
            oled.addressing_range();
            pm.pop();
        } else if (!enabled[selected]) {
            enabled[selected] = true;
            draw_time(oled, shutter_delay[selected]);
        } else if (button == 3) {
            enabled[selected] = false;
            oled.addressing_range();
            oled.clear(2, 6);
            oled.vertical_addressing_mode();
            draw_OFF(oled);
        } else if (op_dir != 0) {
            op_dir = 0;  // cancel operation if multiple buttons are pressed
        } else {
            if (button == 2 && shutter_delay[selected] < MAX_DELAY) {
                op_dir = 1;
            } else if (button == 0 && shutter_delay[selected] > 0) {
                op_dir = -1;
            }

            if (op_dir != 0) {
                shutter_delay[selected] += op_dir;
                op_delay_start = shutter_delay[selected];
                op_start_tick = tick;
                draw_time(oled, shutter_delay[selected]);
            }
        }
    } else if (event == button_event::release) {
        if (button == 2 || button == 0) {
            op_dir = 0;
        }
    }
}

void ui_set_delay::tick(uint32_t tick) {
    if (op_dir != 0 && tick - op_start_tick > 500) {
        int32_t t = tick - op_start_tick - 500;
        int32_t next_value = op_delay_start + op_dir * t / 100;
        if (t > 2000) {
            int32_t t2 = t - 2000;
            next_value += op_dir * (t2 * t2 / 2000);
        }
        if (next_value > MAX_DELAY) {
            next_value = MAX_DELAY;
            op_dir = 0;
        } else if (next_value < 0) {
            next_value = 0;
            op_dir = 0;
        }
        shutter_delay[selected] = next_value;
        draw_time(oled, shutter_delay[selected]);
    }
}
