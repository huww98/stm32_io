#include "drivers.h"
#include "oled.h"
#include "74hc595.h"
#include "timing.h"
#include "settings.h"
#include "ui_set_delay.h"
#include "ui_set_value.h"
#include "ui_trigger.h"
#include "ui_menu.h"
#include "ui_about.h"
#include "utils.h"
#include <cstdio>

oled_driver oled({
    .pwr_port = OLED_PWR_GPIO_Port,
    .pwr_pin = OLED_PWR_Pin,
    .rst_port = OLED_RE__GPIO_Port,
    .rst_pin = OLED_RE__Pin,
    .i2c = &hi2c1,
});

r74hc595_driver shutter_trigger({
    .ser_port = SER_GPIO_Port,
    .ser_pin = SER_Pin,
    .rck_port = RCK_GPIO_Port,
    .rck_pin = RCK_Pin,
    .sck_port = SCK_GPIO_Port,
    .sck_pin = SCK_Pin,
});

camera_trigger_pin_def camera_trigger = {
    .driver = shutter_trigger,
    .focus_port = FOCUS_GPIO_Port,
    .focus_pin = FOCUS_Pin,
};

void test_mode() {
    oled.test_seq();
    oled_text_mode txt(oled);
    txt.write_string("Hello World!\n");
    // txt.write_string("This is a very loooooong line of text.\n");
    // txt.write_string("\ta\nt\ta\nta\ta\ntab\ta\n");
    char str[24];
    while (true) {
        auto tick = HAL_GetTick();
        for (uint8_t i = 0; i < buttons.size(); i++) {
            auto event = buttons[i].update(tick);
            if (event != button_event::none) {
                sprintf(str, "%ld\tButton %d: %d\n", tick, i, int(event));
                txt.write_string(str);
            }
        }
        // delay_us(1000000);
        // HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
        __WFI();
    }
}

timing_t shutter_timing;
settings_t settings;

toast_t toast(oled);

ui_individual_delay ui_i_delay(oled, shutter_trigger, shutter_timing);
ui_set_value ui_base_delay(oled, base_delay_desc{shutter_timing});
ui_set_value ui_focus_advance(oled, focus_advance_desc{shutter_timing});
ui_trigger ui_c_trigger(oled, camera_trigger, shutter_timing);

ui_set_value ui_sleep_timeout(oled, sleep_timeout_desc{settings});
ui_set_value ui_contrast(oled, display_contrast_desc{settings, oled});
ui_about_t ui_about(oled);

std::array<menu_item, 4> settings_menu_items = {
    menu_item{"Sleep Timeout",    []() { pm.push(ui_sleep_timeout); }},
    menu_item{"Display Contrast", []() { pm.push(ui_contrast); }},
    menu_item{"Reset",            []() {
        shutter_timing.reset();
        settings.reset();
        HAL_NVIC_SystemReset();
    }},
    menu_item{"About",            []() { pm.push(ui_about); }},
};
ui_menu settings_menu(oled, "[SETTINGS]", settings_menu_items);

std::array<menu_item, 6> main_menu_items = {
    menu_item{"Individual Delay", []() { pm.push(ui_i_delay); }},
    menu_item{"Base Delay",       []() { pm.push(ui_base_delay); }},
    menu_item{"Focus Advance",    []() { pm.push(ui_focus_advance); }},
    menu_item{"Save Timing",      []() {
        if (shutter_timing.dirty) {
            toast.show("Saving", -1);
            shutter_timing.save();
            toast.reset("Saved");
        } else {
            toast.show("No changes");
        }
    }},
    menu_item{"Trigger!",         []() { pm.push(ui_c_trigger); }},
    menu_item{"Settings",         []() { pm.push(settings_menu); }},
};
ui_menu main_menu(oled, "[CAMERA TRIGGER]", main_menu_items);

extern "C" {
void SystemClock_Config();

void main_loop() {
    settings.load();

    shutter_trigger.reset(24);
    oled.init(settings.contrast);
    for (auto &b : buttons)
        b.init();
    shutter_timing.load();

#ifndef NDEBUG
    if (HAL_GPIO_ReadPin(BTN3_GPIO_Port, BTN3_Pin) == GPIO_PIN_RESET)
        test_mode();
#endif

    pm.init(main_menu);

    uint32_t last_activity = HAL_GetTick();
    while (true) {
        auto tick = HAL_GetTick();
        for (uint8_t i = 0; i < buttons.size(); i++) {
            auto event = buttons[i].update(tick);
            if (event != button_event::none) {
                last_activity = tick;
                pm.current_page().handle_button(i, event, tick);
            }
            if (buttons[i].pressed())
                last_activity = tick;
        }
        pm.current_page().tick(tick);

        if ((tick - last_activity) / 1000 > settings.sleep_timeout) {
            oled.sleep();
            HAL_GPIO_WritePin(STATUS_GPIO_Port, STATUS_Pin, GPIO_PIN_SET);
            delay_us(5);  // wait for the I2C STOP condition to be sent
            auto systick_ctrl = SysTick->CTRL;
            SysTick->CTRL = 0;
            HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

            SystemClock_Config();
            SysTick->CTRL = systick_ctrl & (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
            HAL_GPIO_WritePin(STATUS_GPIO_Port, STATUS_Pin, GPIO_PIN_RESET);
            oled.wake();
            // Ignore the button press that woke us up
            for (auto &b : buttons)
                b.init();
            last_activity = tick;
        } else {
            HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
        }
    }
}
}
