#pragma once
#include <cstdint>
#include <string_view>
#include "ui_set_value.h"

struct settings_t {
    uint8_t contrast = 0x7F;
    uint16_t sleep_timeout = 60;
    bool sleep_enabled = true;

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
    void value(uint16_t t, bool enabled=true) {
        if (settings.sleep_timeout != t || settings.sleep_enabled != enabled) {
            settings.sleep_timeout = t;
            settings.sleep_enabled = enabled;
            settings.save();
        }
    }
};

struct display_contrast_desc {
    static constexpr uint16_t max = 100;
    static constexpr uint8_t scale = 0;
    static constexpr value_input_type type = value_input_type::percent;
    static constexpr std::string_view title_text = "DISPLAY CONTRAST";

    settings_t &settings;

    uint16_t value() const { return static_cast<uint16_t>(settings.contrast) * 100u / 256u; }
    void value(uint16_t t) {
        auto c = t * 256u / 100u;
        if (settings.contrast != c) {
            settings.contrast = c;
            settings.save();
        }
    }
};
