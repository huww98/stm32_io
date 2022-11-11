#pragma once

#include "ui_base.h"
#include "oled.h"

class ui_about_t : public ui_base {
  private:
    oled_driver &oled;

  public:
    ui_about_t(oled_driver &oled)
        : oled(oled) {}

    virtual void draw() override;
};
