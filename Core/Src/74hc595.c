#include "74hc595.h"

static void r74hc595_delay(uint16_t delay)
{
  while (delay--) {
    __NOP();
  }
}

void r74hc595_write(struct r74hc595_handler *handler, uint8_t *data, uint16_t len) {
  for (uint16_t i = 0; i < len; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      HAL_GPIO_WritePin(handler->sck_port, handler->sck_pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(handler->ser_port, handler->ser_pin, (data[i] >> (7 - j)) & 1);
      r74hc595_delay(handler->delay);
      HAL_GPIO_WritePin(handler->sck_port, handler->sck_pin, GPIO_PIN_SET);
      r74hc595_delay(handler->delay);
    }
  }
  HAL_GPIO_WritePin(handler->rck_port, handler->rck_pin, GPIO_PIN_SET);
  r74hc595_delay(handler->delay);
  HAL_GPIO_WritePin(handler->rck_port, handler->rck_pin, GPIO_PIN_RESET);
  r74hc595_delay(handler->delay);
}
