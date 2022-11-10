#pragma once

#include "oled.h"
#include "timing.h"
#include "ui_base.h"

class ui_trigger : public ui_base {
  private:
    oled_driver &oled;
    timing_t &timing;
    uint32_t target_tick = -1;
    uint8_t last_countdown = -1;
    bool last_focused = false;

  public:
    ui_trigger(oled_driver &oled, timing_t &timing) : oled(oled), timing(timing) {}

    virtual void draw() override;
    virtual void handle_button(uint8_t button, button_event event, uint32_t tick) override;
    virtual void tick(uint32_t tick) override;
};
