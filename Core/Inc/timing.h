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
