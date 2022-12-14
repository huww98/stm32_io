#pragma once
#include <cstdint>
#include <string_view>
#include "ui_set_value.h"

struct settings_t {
    uint8_t contrast = 0x7F;
    bool sleep_enabled = true;
    uint16_t sleep_timeout = 60;

    void save();
    void load();
    void reset();
};

struct sleep_timeout_desc {
    static constexpr uint16_t max = 3600;
    static constexpr uint8_t scale = 0;
    static constexpr value_input_type type = value_input_type::time;
    static constexpr std::string_view title_text = "SLEEP TIMEOUT";
    static constexpr std::string_view disabled_text = "Never";

    settings_t &settings;

    uint16_t value() const { return settings.sleep_timeout; }
    bool enabled() const { return settings.sleep_enabled; }
    void value(uint16_t t, bool enabled=true) {
        if (settings.sleep_timeout != t || settings.sleep_enabled != enabled) {
            settings.sleep_timeout = t;
            settings.sleep_enabled = enabled;
            settings.save();
        }
    }
};
static_assert(can_disable<sleep_timeout_desc>);

struct display_contrast_desc {
    static constexpr uint16_t max = 100;
    static constexpr uint8_t scale = 0;
    static constexpr value_input_type type = value_input_type::percent;
    static constexpr std::string_view title_text = "DISPLAY CONTRAST";

    settings_t &settings;
    oled_driver &oled;

    uint16_t value() const { return (static_cast<uint16_t>(settings.contrast) * 100u + 127u) / 255u; }
    uint8_t t2c(uint16_t t) const { return (t * 255u + 50u) / 100u; }
    void value(uint16_t t) {
        auto c = t2c(t);
        if (settings.contrast != c) {
            settings.contrast = c;
            settings.save();
        }
    }
    void preview(uint16_t t) {
        oled.contrast(t2c(t));
    }
};
static_assert(can_preview<display_contrast_desc>);
