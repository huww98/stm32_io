#pragma once

#include <array>
#include <cstdint>

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

    timing_t &timing;

    uint16_t time() const { return timing.base_delay; }
    void time(uint16_t t) {
        timing.dirty |= timing.base_delay != t;
        timing.base_delay = t;
    }
};

struct focus_advance_desc {
    static constexpr uint16_t max = 6000;
    static constexpr uint8_t scale = 2;

    timing_t &timing;

    uint16_t time() const { return timing.focus_advance; }
    void time(uint16_t t) {
        timing.dirty |= timing.focus_advance != t;
        timing.focus_advance = t;
    }
};
