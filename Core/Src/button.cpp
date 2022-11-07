#include "button.h"

button_event button_driver::update(uint32_t current_tick) {
    if (_state == _transit_state)
        return button_event::none;

    if (current_tick - _transit_tick > DEBOUNCE_TIME) {
        _state = _transit_state;
        return _state == GPIO_PinState::GPIO_PIN_SET ? button_event::release : button_event::press;
    }

    return button_event::none;
}

void button_driver::interrupt() {
    auto state = HAL_GPIO_ReadPin(_pin_def.port, _pin_def.pin);
    if (state != _transit_state) {
        _transit_state = state;
        _transit_tick = HAL_GetTick();
    }
}
