#pragma once

#include "main.h"

struct oled_handle {
  GPIO_TypeDef *pwr_port;
  uint16_t pwr_pin;
  GPIO_TypeDef *rst_port;
  uint16_t rst_pin;
  I2C_HandleTypeDef *i2c;
};

void oled_init(struct oled_handle *handle);
void oled_test_seq(I2C_HandleTypeDef *i2c);
