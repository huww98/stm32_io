#pragma once

#include "oled.h"
#include "timing.h"
#include "74hc595.h"
#include "ui_base.h"
#include "power_manager.h"

class ui_trigger : public ui_base {
  private:
    oled_driver &oled;
    camera_trigger_pin_def &camera_trigger;
    timing_t &timing;
    uint32_t target_tick = -1;
    uint8_t last_countdown = -1;
    bool last_focused = false;
    [[no_unique_address]] std::optional<wake_lock> _wake_lock;

    void reset_ui() {
        target_tick = -1;
        last_countdown = -1;
        last_focused = false;
        _wake_lock.reset();
    }

  public:
    ui_trigger(oled_driver &oled, camera_trigger_pin_def &camera_trigger, timing_t &timing)
        : oled(oled), camera_trigger(camera_trigger), timing(timing) {}

    virtual void draw() override;
    virtual void handle_button(uint8_t button, button_event event, uint32_t tick) override;
    virtual void tick(uint32_t tick) override;
};
