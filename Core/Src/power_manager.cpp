#include "main.h"
#include "power_manager.h"
#include "drivers.h"
#include "utils.h"

wake_lock::wake_lock() {
    power_manager::get().wake_lock_count++;
}

wake_lock::~wake_lock() {
    power_manager::get().wake_lock_count--;
}

void power_manager::active(uint32_t tick) {
    this->last_active_tick = tick;
}

extern "C" {
void SystemClock_Config();
}

bool power_manager::try_stop(uint32_t tick) {
    if (!settings.sleep_enabled)
        return false;
    if (this->wake_lock_count > 0) {
        last_active_tick = tick;
        return false;
    }
    if ((tick - last_active_tick) / 1000 > settings.sleep_timeout) {
        oled.sleep();
        HAL_GPIO_WritePin(STATUS_GPIO_Port, STATUS_Pin, GPIO_PIN_SET);
        delay_us(5);  // wait for the I2C STOP condition to be sent
        HAL_SuspendTick();
        HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

        SystemClock_Config();
        HAL_ResumeTick();
        HAL_GPIO_WritePin(STATUS_GPIO_Port, STATUS_Pin, GPIO_PIN_RESET);
        oled.wake();
        // Ignore the button press that woke us up
        for (auto &b : buttons)
            b.init();
        last_active_tick = tick;
        return true;
    }
    return false;
}
