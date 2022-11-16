#pragma once

#include <cstdint>

#include "oled.h"
#include "settings.h"

class wake_lock {
  public:
    wake_lock();
    wake_lock(wake_lock &&) = delete;
    wake_lock(const wake_lock &) = delete;
    ~wake_lock();
};

class power_manager {
  private:
    uint32_t last_active_tick;
    uint32_t timeout;
    uint16_t wake_lock_count = 0;
    oled_driver &oled;
    settings_t &settings;
    friend class wake_lock;

    static power_manager instance;

  public:
    static power_manager &get() {
        return instance;
    }
    power_manager(oled_driver &oled, settings_t &settings): oled(oled), settings(settings) {
    }
    bool try_stop(uint32_t tick);
    void active(uint32_t tick);
};
