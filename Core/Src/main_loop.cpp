#include "drivers.h"
#include "oled.h"
#include "74hc595.h"
#include "ui_set_time.h"
#include "ui_menu.h"
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

void test_mode() {
    oled.test_seq();
    oled_text_mode txt(oled);
    txt.write_string("Hello World!\n");
    // txt.write_string("This is a very loooooong line of text.\n");
    // txt.write_string("\ta\nt\ta\nta\ta\ntab\ta\n");
    char str[24];
    while (1) {
        auto tick = HAL_GetTick();
        for (uint8_t i = 0; i < bottons.size(); i++) {
            auto event = bottons[i].update(tick);
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

extern "C" {
void main_loop() {
    oled.init();

    if (HAL_GPIO_ReadPin(BTN3_GPIO_Port, BTN3_Pin) == GPIO_PIN_RESET)
        test_mode();

    ui_individual_delay ui_i_delay(oled, shutter_trigger);

    std::array<menu_item, 6> main_menu_items = {
        menu_item{"Individual Delay", [&]() { pm.push(ui_i_delay); }},
        menu_item{"Base Delay",       []() { }},
        menu_item{"Focus Advance",    []() { }},
        menu_item{"Save Timming",     []() { }},
        menu_item{"Trigger!",         []() { }},
        menu_item{"Settings",         []() { }},
    };
    ui_menu main_menu(oled, "[CAMERA TRIGGER]", main_menu_items);

    pm.init(main_menu);

    while (1) {
        auto tick = HAL_GetTick();
        for (uint8_t i = 0; i < bottons.size(); i++) {
            auto event = bottons[i].update(tick);
            if (event != button_event::none) {
                pm.current_page().handle_button(i, event, tick);
            }
        }
        pm.current_page().tick(tick);
        __WFI();
    }
}
}
