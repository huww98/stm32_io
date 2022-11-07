#pragma once

#include "main.h"

struct button_pin_def {
    GPIO_TypeDef *port;
    uint16_t pin;
};

enum class button_event {
    none,
    press,
    release,
};

class button_driver {
  private:
    button_pin_def _pin_def;
    GPIO_PinState _state = GPIO_PinState::GPIO_PIN_SET;
    GPIO_PinState _transit_state = GPIO_PinState::GPIO_PIN_SET;
    uint32_t _transit_tick = 0;

  public:
    static constexpr uint32_t DEBOUNCE_TIME = 10;
    button_driver(button_pin_def &&pin_def) : _pin_def(pin_def){};
    const button_pin_def &pin_def() const { return _pin_def; };

    button_event update(uint32_t current_tick);
    void interrupt();
};
