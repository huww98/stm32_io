#pragma once

#include "main.h"
#include <array>
#include <string_view>

const std::array<uint8_t, 6> &get_font6x8(char c);

struct oled_pin_def {
    GPIO_TypeDef *pwr_port;
    uint16_t pwr_pin;
    GPIO_TypeDef *rst_port;
    uint16_t rst_pin;
    I2C_HandleTypeDef *i2c;
};

class oled_driver {
  private:
    oled_pin_def _pin_def;

  public:
    static constexpr uint8_t OLED_I2C_ADDR = 0b111100 << 1;
    oled_driver(oled_pin_def &&pin_def) : _pin_def(pin_def){};
    const oled_pin_def &pin_def() const { return _pin_def; };

    void init(uint8_t contrast=0x7F);
    void clear(uint8_t begin_page=0, uint8_t end_page=8);
    template <size_t N>
    void i2c_transmit(const std::array<uint8_t, N> &data, uint32_t timeout = 1) {
        i2c_transmit(data.data(), N, timeout);
    }
    void i2c_transmit(std::initializer_list<uint8_t> data, uint32_t timeout = 1) {
        i2c_transmit(data.begin(), data.size(), timeout);
    }
    void i2c_transmit(const uint8_t *data, size_t n, uint32_t timeout = 1) {
        HAL_I2C_Master_Transmit(_pin_def.i2c, OLED_I2C_ADDR, (uint8_t *)data, n, timeout);
    }
    void contrast(uint8_t contrast);
    void test_seq();
    void page_addressing_mode();
    void set_pos(uint8_t x, uint8_t y);

    void vertical_addressing_mode();
    void horizontal_addressing_mode();
    void addressing_range(uint8_t begin_page=0, uint8_t end_page=7, uint8_t begin_col=0, uint8_t end_col=127);
};

class oled_text_mode {
  private:
    uint8_t x;
    uint8_t y;
    oled_driver &oled;

  public:
    oled_text_mode(oled_driver &oled);
    void write_string(std::string_view str);
};
