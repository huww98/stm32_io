#pragma once

#include "74hc595.h"
#include "button.h"
#include "oled.h"

class ui_cam_trigger {
  private:
    oled_driver &oled;
    r74hc595_driver &shutter_trigger;
    uint8_t selected = 0;
    std::array<uint16_t, 24> shutter_delay;
    std::array<uint8_t, 24> order;

    void update_order();

  public:
    static constexpr std::string_view title_txt = "CAMERA TRIGGER";

    ui_cam_trigger(oled_driver &oled, r74hc595_driver &shutter_trigger)
        : oled(oled), shutter_trigger(shutter_trigger) {

        shutter_delay.fill(-1);
        update_order();
    };
    void draw_title();
    void draw_cam(int pos);
    void draw();

    void handle_button(uint8_t button, button_event event);
};
