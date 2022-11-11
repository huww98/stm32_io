#pragma once

#include "main.h"
#include <array>

struct r74hc595_pin_def {
    GPIO_TypeDef *ser_port; uint16_t ser_pin;
    GPIO_TypeDef *rck_port; uint16_t rck_pin;
    GPIO_TypeDef *sck_port; uint16_t sck_pin;
};

class r74hc595_driver {
    r74hc595_pin_def _pin_def;
    uint16_t delay;

    public:
        r74hc595_driver(r74hc595_pin_def &&pin_def, uint16_t delay = 32) : _pin_def(pin_def), delay(delay) {};
        template<size_t N>
        void write(const std::array<uint8_t, N> &data) {
            write(data.data(), data.size());
        }
        void write(const uint8_t *data, size_t size);
        void reset(size_t n);
};

struct camera_trigger_pin_def {
    r74hc595_driver &driver;
    GPIO_TypeDef *focus_port;
    uint16_t focus_pin;

    void write_focus(GPIO_PinState state) {
        HAL_GPIO_WritePin(focus_port, focus_pin, state);
    }
};
