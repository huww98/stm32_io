#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include "ui_set_value.h"

struct timing_t {
    constexpr static int NUM_SLOTS = 24;

    std::array<uint16_t, NUM_SLOTS> shutter_delay = {0};
    std::array<bool, NUM_SLOTS> enabled = {false};

    uint16_t base_delay = 200;
    uint16_t focus_advance = 500;

    bool dirty = false;

    void save();
    void load();
    void reset();
};

struct base_delay_desc {
    static constexpr uint16_t max = 6000;
    static constexpr uint8_t scale = 2;
    static constexpr value_input_type type = value_input_type::time;
    static constexpr std::string_view title_text = "BASE DELAY";

    timing_t &timing;

    uint16_t value() const { return timing.base_delay; }
    void value(uint16_t t) {
        timing.dirty |= timing.base_delay != t;
        timing.base_delay = t;
    }
};

struct focus_advance_desc {
    static constexpr uint16_t max = 6000;
    static constexpr uint8_t scale = 2;
    static constexpr value_input_type type = value_input_type::time;
    static constexpr std::string_view title_text = "FOCUS ADVANCE";

    timing_t &timing;

    uint16_t value() const { return timing.focus_advance; }
    void value(uint16_t t) {
        timing.dirty |= timing.focus_advance != t;
        timing.focus_advance = t;
    }
};
