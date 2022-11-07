#pragma once

#include "oled.h"
#include "button.h"

class ui_cam_trigger {
  private:
    oled_driver &oled;
    uint8_t selected = 0;

  public:
    static constexpr std::string_view title_txt = "CAMERA TRIGGER";

    ui_cam_trigger(oled_driver &oled) : oled(oled) {}
    void draw_title();
    void draw_cam(int pos, int order);
    void draw();

    void handle_button(uint8_t button, button_event event);
};
