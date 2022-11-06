#pragma once

#include <array>
#include <string_view>
#include "main.h"

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
  oled_driver(oled_pin_def &&pin_def) : _pin_def(pin_def) {};
  const oled_pin_def &pin_def() const { return _pin_def; };

  void init();
  void clear();
  template<size_t N>
  void i2c_transmit(const std::array<uint8_t, N> &data, uint32_t timeout=1);
  void test_seq();
  void page_addressing_mode();
  void set_pos(uint8_t x, uint8_t y);
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
