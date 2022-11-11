#pragma once
#include <cstdint>

struct settings_t {
    uint8_t contrast = 0x7F;
    uint16_t sleep_timeout = 60;

    bool dirty = false;

    void save();
    void load();
    void reset();
};
