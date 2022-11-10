#include "ui_trigger.h"
#include "fonts.h"

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
            target_tick = -1;
            last_countdown = -1;
            last_focused = false;
        }
    }
    ui_base::handle_button(button, event, tick);
}

void ui_trigger::tick(uint32_t tick) {
    if (target_tick == static_cast<decltype(target_tick)>(-1))
        return;

    int32_t remaining = target_tick - static_cast<int32_t>(tick);
    auto countdown = (remaining - 1) / 1000 + 1;
    auto focused = remaining / 10 < timing.focus_advance;

    if (this->target_tick <= tick || countdown != last_countdown || focused != last_focused) {
        last_countdown = countdown;
        last_focused = focused;

        auto data = this->target_tick < tick ? SHUTTER_CIRCLE_BITS : SHUTTER_RING_BITS;
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
    }
}
