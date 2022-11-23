#include "ui_about.h"
#include "ui_menu.h"
#include "version.h"

void ui_about_t::draw() {
    oled.clear(0, 2);
    put_string_center(oled, "Camera Trigger", 2);

    oled.clear(3, 4);
    put_string_center(oled, GIT_VERSION, 4);
    put_string_center(oled, "by: huww98", 5);
    oled.clear(6);
}
