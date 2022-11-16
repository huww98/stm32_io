#include "ui_trigger.h"
#include "fonts.h"
#include "power_manager.h"
#include <algorithm>

constexpr int FOCUS_RO = 24;
constexpr int FOCUS_RI = 22;
constexpr int SHUTTER_RO = 20;
constexpr int SHUTTER_RI = 19;

constexpr auto get_circle_bits(int ro, int ri) {
    constexpr int D = 48;

    ro <<= 1;
    ri <<= 1;
    int ro_square = ro * ro;
    int ri_square = ri * ri;
    int c = D - 1;

    std::array<uint8_t, D * D / 8 + 1> data{0};
    data[0] = 0x40;
    for (int y = 0; y < D; y++) {
        for (int x = 0; x < D; x++) {
            int byte = (y >> 3) + x * (D / 8);
            int bit = y & 0b111;

            int dx = (x << 1) - c;
            int dy = (y << 1) - c;
            int d_square = dx * dx + dy * dy;
            if (d_square < ro_square && d_square > ri_square) {
                data[byte + 1] |= 1 << bit;
            }
        }
    }
    return data;
}

constexpr auto FOCUS_RING_BITS = get_circle_bits(FOCUS_RO, FOCUS_RI);
constexpr auto SHUTTER_RING_BITS = get_circle_bits(SHUTTER_RO, SHUTTER_RI);
constexpr auto SHUTTER_CIRCLE_BITS = get_circle_bits(SHUTTER_RO, 0);

void ui_trigger::draw() {
    oled.clear();

    oled.vertical_addressing_mode();
    oled.addressing_range(1, 6, (128 - 48) / 2, (128 - 48) / 2 + 48 - 1);
    oled.i2c_transmit(SHUTTER_RING_BITS, 50);
}

void ui_trigger::handle_button(uint8_t button, button_event event, uint32_t tick) {
    if (event == button_event::press) {
        if (button == 1) {
            this->target_tick = tick + timing.base_delay * 10;
            return;
        } else if (button == 3) {
            oled.addressing_range();
            this->reset_ui();
            camera_trigger.write_focus(GPIO_PIN_SET);
        }
    }
    ui_base::handle_button(button, event, tick);
}

namespace {

struct trigger_action {
    enum class action_type {
        none,
        trigger_shutter,
        release_shutter,
    } action;
    uint8_t slot;
    uint32_t n_reload;
    uint32_t sys_tick;
};

struct trigger_step {
    std::array<uint8_t, 3> shutter_bits;
    uint32_t n_reload;
    uint32_t sys_tick;
};

template<typename IT>
IT build_steps(timing_t &timing, IT steps_begin) {
    auto systick_reload = SysTick->LOAD;
    std::array<trigger_action, 48> actions;

    auto it = actions.begin();
    for (uint8_t i = 0; i < timing.shutter_delay.size(); i++) {
        if (!timing.enabled[i])
            continue;
        uint16_t delay = timing.shutter_delay[i];
        auto tick = systick_reload - (delay % 10) * systick_reload / 10;
        uint16_t n_reload = delay / 10;
        *it++ = {
            .action = trigger_action::action_type::trigger_shutter,
            .slot = i,
            .n_reload = n_reload,
            .sys_tick = tick,
        };
        *it++ = {
            .action = trigger_action::action_type::release_shutter,
            .slot = i,
            .n_reload = n_reload + 100u,
            .sys_tick = tick,
        };
    }

    auto end = it;
    it = actions.begin();
    std::sort(actions.begin(), end, [](auto &a, auto &b) {
        return a.n_reload < b.n_reload || (a.n_reload == b.n_reload && a.sys_tick > b.sys_tick);
    });

    auto it_s = steps_begin;

    while (it != end) {
        it_s->n_reload = it->n_reload;
        it_s->sys_tick = it->sys_tick;
        if (it_s == steps_begin)
            std::fill(it_s->shutter_bits.begin(), it_s->shutter_bits.end(), 0xFF);
        else
            it_s->shutter_bits = std::prev(it_s)->shutter_bits;

        while (it != end && it_s->n_reload == it->n_reload && it_s->sys_tick == it->sys_tick) {
            uint8_t mask = 1 << it->slot % 8;
            uint8_t &byte = it_s->shutter_bits[it->slot / 8];

            switch (it->action) {
            case trigger_action::action_type::trigger_shutter:
                byte &= ~mask;
                break;
            case trigger_action::action_type::release_shutter:
                byte |= mask;
                break;
            default:
                break;
            }
            it++;
        }
        it_s++;
    }

    return it_s;
}

void trigger_shutter(r74hc595_driver &driver, timing_t &timing, uint32_t target_tick) {
    std::array<trigger_step, 48> steps;
    auto steps_end = build_steps(timing, steps.begin());

    __disable_irq();
    // delay to the actual target tick
    [[maybe_unused]] auto dummy = SysTick->CTRL; // ensure COUNTFLAG is cleared
    auto current_tick = HAL_GetTick();
    auto pending_tick = target_tick - current_tick;
    while (pending_tick) {
        if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
            pending_tick--;
    }
    uint32_t n_reload = 0;

    for (auto it = steps.begin(); it != steps_end; it++) {
        while (n_reload < it->n_reload || (n_reload == it->n_reload && SysTick->VAL > it->sys_tick)) {
            if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
                n_reload++;
        }

        driver.write(it->shutter_bits);
    }
    __enable_irq();
}

}

void ui_trigger::tick(uint32_t tick) {
    if (target_tick == static_cast<decltype(target_tick)>(-1))
        return;

    int32_t remaining = target_tick - static_cast<int32_t>(tick);
    auto countdown = (remaining - 1) / 1000 + 1;
    auto focused = remaining / 10 < timing.focus_advance;
    auto target_tick = this->target_tick - 10; // reserve 10ms for task preparation

    if (target_tick < tick || countdown != last_countdown || focused != last_focused) {
        if (!_wake_lock.has_value())
            _wake_lock.emplace();
        if (focused != last_focused) {
            assert(focused && !last_focused);
            camera_trigger.write_focus(GPIO_PIN_RESET);
        }

        last_countdown = countdown;
        last_focused = focused;

        auto data = target_tick < tick ? SHUTTER_CIRCLE_BITS : SHUTTER_RING_BITS;
        if (focused) {
            for (size_t i = 1; i < data.size(); i++)
                data[i] |= FOCUS_RING_BITS[i];
        }

        auto it = data.begin() + 1;
        auto add_char = [&it](char c) {
            auto &font = get_font_ter_u32b(c);
            for (int i = 0; i < 16; i++) {
                it++;
                for (int j = 0; j < 4; j++)
                    *it++ |= font[i * 4 + j];
                it++;
            }
        };
        it += 6 * 8;
        if (countdown >= 10) {
            add_char('0' + countdown / 10);
            add_char('0' + countdown % 10);
        } else {
            it += 6 * 8;
            add_char('0' + countdown);
        }

        oled.i2c_transmit(data, 50);

        if (target_tick < tick) {
            trigger_shutter(camera_trigger.driver, timing, this->target_tick);
            camera_trigger.write_focus(GPIO_PIN_SET);
            this->reset_ui();
            this->draw();
        }
    }
}
