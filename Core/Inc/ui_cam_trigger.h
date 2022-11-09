#pragma once

#include "74hc595.h"
#include "button.h"
#include "oled.h"
#include "ui_base.h"

class ui_set_delay : public ui_base {
  private:
    oled_driver &oled;
    std::array<uint16_t, 24> &shutter_delay;
    std::array<bool, 24> &enabled;
    uint8_t selected = 0;

  public:
    static constexpr std::string_view title_txt = "SET SHUTTER DELAY";
    static constexpr uint16_t MAX_DELAY = 50000;

    ui_set_delay(oled_driver &oled, std::array<uint16_t, 24> &shutter_delay, std::array<bool, 24> &enabled)
        : oled(oled), shutter_delay(shutter_delay), enabled(enabled) {}

    void select(uint8_t pos) {
        selected = pos;
    }

    virtual void draw() override;
    virtual void handle_button(uint8_t button, button_event event) override;
};

class ui_cam_trigger : public ui_base {
  private:
    oled_driver &oled;
    r74hc595_driver &shutter_trigger;
    uint8_t selected = 0;
    std::array<uint16_t, 24> shutter_delay = {0};
    std::array<bool, 24> enabled = {false};
    std::array<uint8_t, 24> order;

    ui_set_delay set_delay_page;

    void update_order();

  public:
    static constexpr std::string_view title_txt = "CAMERA TRIGGER";

    ui_cam_trigger(oled_driver &oled, r74hc595_driver &shutter_trigger)
        : oled(oled), shutter_trigger(shutter_trigger), set_delay_page(oled, shutter_delay, enabled) {
    };
    void draw_cam(int pos);
    virtual void draw() override;
    virtual void handle_button(uint8_t button, button_event event) override;
};
