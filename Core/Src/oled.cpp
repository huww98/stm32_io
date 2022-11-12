#include "oled.h"
#include "utils.h"

constexpr std::array<uint8_t, 6> font6x8[] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // sp
    {0x00, 0x00, 0x00, 0x2f, 0x00, 0x00}, // !
    {0x00, 0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14}, // #
    {0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12}, // $
    {0x00, 0x62, 0x64, 0x08, 0x13, 0x23}, // %
    {0x00, 0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x00, 0x1c, 0x22, 0x41, 0x00}, // (
    {0x00, 0x00, 0x41, 0x22, 0x1c, 0x00}, // )
    {0x00, 0x14, 0x08, 0x3E, 0x08, 0x14}, // *
    {0x00, 0x08, 0x08, 0x3E, 0x08, 0x08}, // +
    {0x00, 0x00, 0x00, 0xA0, 0x60, 0x00}, // ,
    {0x00, 0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x00, 0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x00, 0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x00, 0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x00, 0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x00, 0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x00, 0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x00, 0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x00, 0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    {0x00, 0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x00, 0x08, 0x14, 0x22, 0x41, 0x00}, // <
    {0x00, 0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x00, 0x00, 0x41, 0x22, 0x14, 0x08}, // >
    {0x00, 0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x00, 0x32, 0x49, 0x59, 0x51, 0x3E}, // @
    {0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C}, // A
    {0x00, 0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x00, 0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x00, 0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x00, 0x7F, 0x09, 0x09, 0x09, 0x01}, // F
    {0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
    {0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x00, 0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x00, 0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x00, 0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
    {0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x00, 0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x00, 0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x00, 0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x00, 0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
    {0x00, 0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x00, 0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x00, 0x61, 0x51, 0x49, 0x45, 0x43}, // Z
    {0x00, 0x00, 0x7F, 0x41, 0x41, 0x00}, // [
    {0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55}, // 55
    {0x00, 0x00, 0x41, 0x41, 0x7F, 0x00}, // ]
    {0x00, 0x04, 0x02, 0x01, 0x02, 0x04}, // ^
    {0x00, 0x40, 0x40, 0x40, 0x40, 0x40}, // _
    {0x00, 0x00, 0x01, 0x02, 0x04, 0x00}, // '
    {0x00, 0x20, 0x54, 0x54, 0x54, 0x78}, // a
    {0x00, 0x7F, 0x48, 0x44, 0x44, 0x38}, // b
    {0x00, 0x38, 0x44, 0x44, 0x44, 0x20}, // c
    {0x00, 0x38, 0x44, 0x44, 0x48, 0x7F}, // d
    {0x00, 0x38, 0x54, 0x54, 0x54, 0x18}, // e
    {0x00, 0x08, 0x7E, 0x09, 0x01, 0x02}, // f
    {0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C}, // g
    {0x00, 0x7F, 0x08, 0x04, 0x04, 0x78}, // h
    {0x00, 0x00, 0x44, 0x7D, 0x40, 0x00}, // i
    {0x00, 0x40, 0x80, 0x84, 0x7D, 0x00}, // j
    {0x00, 0x7F, 0x10, 0x28, 0x44, 0x00}, // k
    {0x00, 0x00, 0x41, 0x7F, 0x40, 0x00}, // l
    {0x00, 0x7C, 0x04, 0x18, 0x04, 0x78}, // m
    {0x00, 0x7C, 0x08, 0x04, 0x04, 0x78}, // n
    {0x00, 0x38, 0x44, 0x44, 0x44, 0x38}, // o
    {0x00, 0xFC, 0x24, 0x24, 0x24, 0x18}, // p
    {0x00, 0x18, 0x24, 0x24, 0x18, 0xFC}, // q
    {0x00, 0x7C, 0x08, 0x04, 0x04, 0x08}, // r
    {0x00, 0x48, 0x54, 0x54, 0x54, 0x20}, // s
    {0x00, 0x04, 0x3F, 0x44, 0x40, 0x20}, // t
    {0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C}, // u
    {0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C}, // v
    {0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C}, // w
    {0x00, 0x44, 0x28, 0x10, 0x28, 0x44}, // x
    {0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C}, // y
    {0x00, 0x44, 0x64, 0x54, 0x4C, 0x44}, // z
};

const std::array<uint8_t, 6> &get_font6x8(char c) { return font6x8[c - ' ']; }

void oled_driver::contrast(uint8_t contrast) {
    i2c_transmit({0x00, 0x81, contrast});
}

void oled_driver::clear(uint8_t begin_page, uint8_t end_page) {
    std::array<uint8_t, 129> data;
    data[0] = 0x40;
    std::fill(data.begin() + 1, data.end(), 0);

    page_addressing_mode();
    for (uint8_t i = begin_page; i < end_page; i++) {
        set_pos(0, i);
        i2c_transmit(data, 10);
    }
}

void oled_driver::test_seq() {
    i2c_transmit(std::array<uint8_t, 6>({0,
        0x20, 0x00, // horizontal addressing mode
        0x22, 0, 7,    // set page start/end
    }));

    std::array<uint8_t, 2> data;
    data[0] = 0x40;
    for (int m = 0; m < 2; m++) {
        if (m == 0)
            data[1] = 0xff;
        else
            data[1] = 0x00;

        for (int i = 0; i < 8; i++) {
            for (int j = 1; j < 129; j++) {
                i2c_transmit(data);
                HAL_Delay(2);
            }
        }
    }
}

void oled_driver::init(uint8_t contrast) {
    HAL_GPIO_WritePin(_pin_def.rst_port, _pin_def.rst_pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(_pin_def.pwr_port, _pin_def.pwr_pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(_pin_def.rst_port, _pin_def.rst_pin, GPIO_PIN_SET);
    delay_us(5);

    clear();

    i2c_transmit({
        0x00, 0xA1, 0xC8, // rotate display 180 degrees
        0x81, contrast,   // set contrast
        0x8D, 0x14,       // enable charge pump
        0xAF,             // display on
    });
}

void oled_driver::page_addressing_mode() { i2c_transmit(std::array<uint8_t, 3>{0x00, 0x20, 0b10}); }

void oled_driver::set_pos(uint8_t x, uint8_t y) {
    i2c_transmit(std::array<uint8_t, 4>{0x00,
        static_cast<uint8_t>(0xb0 + y),
        static_cast<uint8_t>(x & 0xf),
        static_cast<uint8_t>(0x10 | (x >> 4))
    });
}

void oled_driver::vertical_addressing_mode() {
    i2c_transmit(std::to_array<uint8_t>({0x00, 0x20, 0b01}));
}
void oled_driver::horizontal_addressing_mode() {
    i2c_transmit(std::to_array<uint8_t>({0x00, 0x20, 0b00}));
}


void oled_driver::addressing_range(uint8_t begin_page, uint8_t end_page, uint8_t begin_col, uint8_t end_col) {
    i2c_transmit(std::to_array<uint8_t>({0x00,
        0x21, begin_col, end_col,
        0x22, begin_page, end_page,
    }));
}

oled_text_mode::oled_text_mode(oled_driver &oled) : x(0), y(0), oled(oled) {
    oled.page_addressing_mode();
    oled.set_pos(0, 0);
}

void oled_text_mode::write_string(std::string_view str_view) {
    for (char c : str_view) {
        if (c == '\n') {
            this->x = 0;
            this->y++;
            if (this->y > 7)
                this->y = 0;
            oled.set_pos(this->x, this->y);
        } else if (c == '\r') {
            this->x = 0;
            oled.set_pos(this->x, this->y);
        } else if (c == '\t') {
            this->x = this->x + 4 * 6 - this->x % (4 * 6);
            if (this->x > 120) {
                this->x = 0;
                this->y++;
                if (this->y > 7)
                    this->y = 0;
            }
            oled.set_pos(this->x, this->y);
        } else {
            std::array<uint8_t, 7> data;
            data[0] = 0x40;
            if (c >= ' ' && c <= 'z') {
                auto &font = get_font6x8(c);
                for (int i = 0; i < 6; i++)
                    data[i + 1] = font[i];
            } else {
                auto &font = get_font6x8('?');
                for (int i = 0; i < 6; i++)
                    data[i + 1] = ~font[i];
            }
            oled.i2c_transmit(data);
            this->x += 6;
            if (this->x > 120) {
                this->x = 0;
                this->y++;
                if (this->y > 7) {
                    this->y = 0;
                }
                oled.set_pos(this->x, this->y);
            }
        }
    }
}
