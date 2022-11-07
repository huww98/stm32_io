#include "drivers.h"
#include "button.h"
#include "int_handlers.h"

#include <array>

std::array<button_driver, 4> bottons{
    button_driver{{BTN0_GPIO_Port, BTN0_Pin}},
    button_driver{{BTN1_GPIO_Port, BTN1_Pin}},
    button_driver{{BTN2_GPIO_Port, BTN2_Pin}},
    button_driver{{BTN3_GPIO_Port, BTN3_Pin}},
};

extern "C" {

void btn_interrupt_handler() {
    for (auto &btn : bottons)
        btn.interrupt();
}
}
