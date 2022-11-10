#include "ui_base.h"

page_manager pm;

void ui_base::handle_button(uint8_t button, button_event event, uint32_t tick) {
    if (event == button_event::press && button == 3) {
        pm.pop();
    }
}
