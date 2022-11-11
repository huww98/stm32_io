#pragma once
#include <cstdint>

struct settings_t {
    uint8_t contrast = 0x7F;
    uint16_t sleep_timeout = 60;

    void save();
    void load();
    void reset();
};

struct sleep_timeout_desc {
    static constexpr uint16_t max = 3600;
    static constexpr uint8_t scale = 0;

    settings_t &settings;

    uint16_t time() const { return settings.sleep_timeout; }
    void time(uint16_t t) {
        if (settings.sleep_timeout != t) {
            settings.sleep_timeout = t;
            settings.save();
        }
    }
};
