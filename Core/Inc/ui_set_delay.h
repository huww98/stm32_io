#pragma once
#include "ui_base.h"
#include "timing.h"

class ui_set_delay : public ui_base {
  private:
    oled_driver &oled;
    timing_t &timing;
    uint8_t selected = 0;

    value_input _time_input;

  public:
    static constexpr std::string_view title_txt = "SET DELAY";
    static constexpr uint16_t MAX_DELAY = 50000;

    ui_set_delay(oled_driver &oled, timing_t &timing)
        : oled(oled), timing(timing), _time_input(oled, 4, MAX_DELAY) {
    }

    void select(uint8_t pos) {
        selected = pos;
        _time_input.time = timing.shutter_delay[selected];
    }

    virtual void draw() override;
    virtual void handle_button(uint8_t button, button_event event, uint32_t tick) override;
    virtual void tick(uint32_t tick) override;
};

class ui_individual_delay : public ui_base {
  private:
    oled_driver &oled;
    r74hc595_driver &shutter_trigger;
    timing_t &timing;
    uint8_t selected = 0;
    std::array<uint8_t, 24> order;

    ui_set_delay set_delay_page;

    void update_order();

  public:
    static constexpr std::string_view title_txt = "INDIVIDUAL DELAY";

    ui_individual_delay(oled_driver &oled, r74hc595_driver &shutter_trigger, timing_t &timing)
        : oled(oled), shutter_trigger(shutter_trigger), timing(timing), set_delay_page(oled, timing) {}

    void draw_cam(int pos);
    virtual void draw() override;
    virtual void handle_button(uint8_t button, button_event event, uint32_t tick) override;
};
