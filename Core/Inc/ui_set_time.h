#pragma once

#include "74hc595.h"
#include "button.h"
#include "oled.h"
#include "timing.h"
#include "ui_base.h"
#include "ui_menu.h"

class time_input {
  private:
    oled_driver &oled;
    uint8_t scale;
    uint8_t num_digits;
    uint16_t max_time;
    std::string_view unit;
    uint8_t padding_left;

    uint16_t op_delay_start;
    uint32_t op_start_tick;
    int op_dir = 0;

  public:
    uint16_t time;

    time_input(oled_driver &oled, uint8_t scale=4, uint16_t max_time=50000)
        : oled(oled), scale(scale), max_time(max_time) {

        num_digits = 0;
        {
            auto max_time = this->max_time;
            while (max_time) {
                max_time /= 10;
                num_digits++;
            }
        }
        unit = "s";
        if (this->scale >= 3) {
            unit = "ms";
            this->scale -= 3;
        }
        int num_chars = num_digits + unit.size();
        if (this->scale > 0)
            num_chars++;

        padding_left = (128 - 16 * num_chars) / 2;
    }

    void handle_button(uint8_t button, button_event event, uint32_t tick);
    void tick(uint32_t tick);
    void draw();
};

template<typename TimeDesc>
class ui_set_time : public ui_base {
  private:
    oled_driver &oled;
    std::string_view title;
    time_input _time_input;
  public:
    TimeDesc time_desc;

    ui_set_time(oled_driver &oled, std::string_view title, TimeDesc &&desc)
        : oled(oled), title(title), _time_input(oled, desc.scale, desc.max), time_desc(desc) {}

    virtual void draw() override {
        oled.page_addressing_mode();
        put_string_center(oled, title, 0, true);

        oled.clear(1);
        oled.vertical_addressing_mode();

        _time_input.time = time_desc.time();
        _time_input.draw();
    }

    virtual void handle_button(uint8_t button, button_event event, uint32_t tick) override {
        if (event == button_event::press) {
            if (button == 3) {
                oled.addressing_range();
                time_desc.time(_time_input.time);
                pm.pop();
                return;
            }
        }
        _time_input.handle_button(button, event, tick);
    }

    virtual void tick(uint32_t tick) override {
        _time_input.tick(tick);
    }
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
