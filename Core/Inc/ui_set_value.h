#pragma once

#include "74hc595.h"
#include "button.h"
#include "oled.h"
#include "ui_base.h"
#include "ui_menu.h"
#include <functional>

enum class value_input_type {
    none,
    time,
    percent,
};

class value_input {
  private:
    oled_driver &oled;
    uint8_t scale;
    uint8_t num_digits;
    uint16_t max_time;
    std::string_view unit;
    std::string_view disabled_text;
    uint8_t padding_left;

    uint16_t op_delay_start;
    uint32_t op_start_tick;
    int op_dir = 0;

  public:
    uint16_t value;
    bool enabled = true;
    std::function<void(uint16_t v)> on_change;

    value_input(oled_driver &oled, uint8_t scale=4, uint16_t max_time=50000, value_input_type type=value_input_type::time, std::string_view disabled_text="")
        : oled(oled), scale(scale), max_time(max_time), disabled_text(disabled_text) {

        num_digits = 0;
        {
            auto max_time = this->max_time;
            while (max_time) {
                max_time /= 10;
                num_digits++;
            }
        }
        switch (type) {
            case value_input_type::time:
                unit = "s";
                if (this->scale >= 3) {
                    unit = "ms";
                    this->scale -= 3;
                }
                break;
            case value_input_type::percent:
                unit = "%";
                break;
            default:
                unit = "";
                break;
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

template<typename T>
concept value_desc = requires(T t) {
    { t.title_text } -> std::convertible_to<std::string_view>;
    { t.max } -> std::convertible_to<uint16_t>;
    { t.scale } -> std::convertible_to<uint8_t>;
    { t.type } -> std::convertible_to<value_input_type>;
    { t.value() } -> std::convertible_to<uint16_t>;
};

template<typename T>
concept can_disable = requires(T t, uint16_t v, bool enabled) {
    { t.disabled_text } -> std::convertible_to<std::string_view>;
    { t.enabled() } -> std::convertible_to<bool>;
    t.value(v, enabled);
};

template<typename T>
concept can_preview = requires(T t, uint16_t v) {
    t.preview(v);
};

template<typename T>
constexpr std::string_view disabled_text(const T &desc) {
    if constexpr (can_disable<T>)
        return desc.disabled_text;
    else
        return "";
}

template<value_desc TimeDesc>
class ui_set_value : public ui_base {
  private:
    oled_driver &oled;
    value_input _value_input;
  public:
    TimeDesc value_desc;

    ui_set_value(oled_driver &oled, TimeDesc &&desc)
        : oled(oled), _value_input(oled, desc.scale, desc.max, desc.type, disabled_text(desc)), value_desc(desc) {

        auto &this_desc = this->value_desc;
        _value_input.on_change = [&this_desc](uint16_t v) {
            if constexpr (can_preview<TimeDesc>)
                this_desc.preview(v);
        };
    }

    virtual void draw() override {
        oled.page_addressing_mode();
        put_string_center(oled, value_desc.title_text, 0, true);

        oled.clear(1);
        oled.vertical_addressing_mode();



        _value_input.value = value_desc.value();
        _value_input.draw();
    }

    virtual void handle_button(uint8_t button, button_event event, uint32_t tick) override {
        if (event == button_event::press) {
            if (button == 3) {
                oled.addressing_range();
                if constexpr (can_disable<TimeDesc>)
                    value_desc.value(_value_input.value, _value_input.enabled);
                else
                    value_desc.value(_value_input.value);
                pm.pop();
                return;
            }
        }
        _value_input.handle_button(button, event, tick);
    }

    virtual void tick(uint32_t tick) override {
        _value_input.tick(tick);
    }
};
