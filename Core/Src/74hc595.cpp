#include "74hc595.h"

static void r74hc595_delay(uint16_t delay) {
    while (delay--) {
        __NOP();
    }
}

void r74hc595_driver::write(uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            HAL_GPIO_WritePin(_pin_def.sck_port, _pin_def.sck_pin, GPIO_PIN_RESET);
            auto state = (data[i] & (1 << (7 - j))) ? GPIO_PIN_SET : GPIO_PIN_RESET;
            HAL_GPIO_WritePin(_pin_def.ser_port, _pin_def.ser_pin, state);
            r74hc595_delay(delay);
            HAL_GPIO_WritePin(_pin_def.sck_port, _pin_def.sck_pin, GPIO_PIN_SET);
            r74hc595_delay(delay);
        }
    }
    HAL_GPIO_WritePin(_pin_def.rck_port, _pin_def.rck_pin, GPIO_PIN_SET);
    r74hc595_delay(delay);
    HAL_GPIO_WritePin(_pin_def.rck_port, _pin_def.rck_pin, GPIO_PIN_RESET);
    r74hc595_delay(delay);
}
