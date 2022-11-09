#pragma once

#include "74hc595.h"
#include "button.h"
#include "oled.h"
#include "timing.h"
#include "ui_base.h"

class time_input {
  private:
    oled_driver &oled;
    uint16_t max_time;

    uint16_t op_delay_start;
    uint32_t op_start_tick;
    int op_dir = 0;

  public:
    uint16_t time;

    time_input(oled_driver &oled, uint16_t max_time=50000) : oled(oled), max_time(max_time) {}

    void handle_button(uint8_t button, button_event event, uint32_t tick);
    void tick(uint32_t tick);
    void draw();
};

class ui_set_time : public ui_base {
  private:
    oled_driver &oled;
    std::string_view title;
    time_input _time_input;
  public:
    uint16_t &time;
    bool &dirty;

    ui_set_time(oled_driver &oled, std::string_view title, uint16_t &time, bool &dirty)
        : oled(oled), title(title), _time_input(oled), time(time), dirty(dirty) {}

    virtual void draw() override;
    virtual void handle_button(uint8_t button, button_event event, uint32_t tick) override;
    virtual void tick(uint32_t tick) override;
};

class ui_set_delay : public ui_base {
  private:
    oled_driver &oled;
    timing_t &timing;
    uint8_t selected = 0;

    time_input _time_input;

  public:
    static constexpr std::string_view title_txt = "SET DELAY";
    static constexpr uint16_t MAX_DELAY = 50000;

    ui_set_delay(oled_driver &oled, timing_t &timing)
        : oled(oled), timing(timing), _time_input(oled, MAX_DELAY) {
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