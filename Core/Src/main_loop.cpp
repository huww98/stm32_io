#include "drivers.h"
#include "oled.h"
#include "74hc595.h"
#include "ui_cam_trigger.h"
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

extern "C" {
void main_loop() {
    oled.init();

    ui_cam_trigger ui(oled, shutter_trigger);
    ui.draw();

    // oled_text_mode txt(oled);
    // txt.write_string("Hello World!\n");
    // txt.write_string("This is a very loooooong line of text.\n");
    // txt.write_string("\ta\nt\ta\nta\ta\ntab\ta\n");
    // char str[24];
    // uint32_t ticks[6];

    // delay_us(400);
    // for (uint8_t i = 0; i < 6; i++) {
    //   delay_us(0);
    //   ticks[i] = SysTick->VAL;
    // }
    // txt.write_string(str);
    // for (uint8_t i = 0; i < 6; i++) {
    //   sprintf(str, "\t%lu\n", ticks[i]);
    //   txt.write_string(str);
    // }

    while (1) {
        auto tick = SysTick->VAL;
        for (uint8_t i = 0; i < bottons.size(); i++) {
            auto event = bottons[i].update(tick);
            if (event != button_event::none) {
                ui.handle_button(i, event);
            }
        }
        // delay_us(1000000);
        // HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
        // oled_test_seq(oled.i2c);
        __WFI();
    }
}
}
