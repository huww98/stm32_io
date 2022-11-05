#include "main.h"

struct r74hc595_handler {
  GPIO_TypeDef *ser_port; uint16_t ser_pin;
  GPIO_TypeDef *rck_port; uint16_t rck_pin;
  GPIO_TypeDef *sck_port; uint16_t sck_pin;
  uint16_t delay;
};

void r74hc595_write(struct r74hc595_handler *handler, uint8_t *data, uint16_t len);
