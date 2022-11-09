#pragma once

#include "ui_base.h"
#include "oled.h"

#include <functional>

struct menu_item {
    std::string_view name;
    std::function<void()> action;
};

class ui_menu : public ui_base {
  private:
    oled_driver &oled;
    const menu_item *menu_items = nullptr;
    size_t menu_items_count = 0;
    std::string_view title;
    uint8_t selected = 0;

  public:
    template <size_t N>
    constexpr ui_menu(oled_driver &oled, std::string_view title, const std::array<menu_item, N> &items)
        : oled(oled), title(title) {

        set_items(items.data(), items.size());
    }

    void set_items(const menu_item *items, size_t count) {
        menu_items = items;
        menu_items_count = count;
    }

    void draw_item(uint8_t index);

    virtual void draw() override;
    virtual void handle_button(uint8_t button, button_event event, uint32_t tick) override;
};
