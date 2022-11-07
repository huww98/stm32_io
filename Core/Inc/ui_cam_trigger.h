#pragma once

#include "oled.h"

class ui_cam_trigger {
  private:
    oled_driver &oled;
    uint8_t selected = 0;

  public:
    static constexpr std::string_view title_txt = "CAMERA TRIGGER";

    ui_cam_trigger(oled_driver &oled) : oled(oled) {}
    void draw_title();
    void draw_cam(int pos, int order, bool selected);
    void draw();
};
