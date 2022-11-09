#pragma once

#include "ui_base.h"
#include "oled.h"

#include <functional>

void put_string(oled_driver &oled, std::string_view str, uint8_t x, uint8_t y, bool item, bool invert = false);
void put_string_center(oled_driver &oled, std::string_view str, uint8_t y, bool invert = false);
void put_string_no_fill(oled_driver &oled, std::string_view str, uint8_t x, uint8_t y, bool invert = false);
void put_string_no_fill_center(oled_driver &oled, std::string_view str, uint8_t y, bool invert = false);

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

class toast_t : public ui_base {
  private:
    oled_driver &oled;
    std::string_view text;
    uint32_t target_tick = DURATION_INFINITY;

  public:
    static constexpr uint32_t DURATION_INFINITY = -1;

    static constexpr int PADDING_X = 16;
    static constexpr int PADDING_TOP = 2;
    static constexpr int PADDING_BOTTOM = 3;
    static constexpr int WIDTH = 128 - PADDING_X * 2;
    static constexpr int TEXT_LINE = PADDING_TOP + (8 - PADDING_TOP - PADDING_BOTTOM) / 2;

    toast_t(oled_driver &oled) : oled(oled) {}
    void reset(std::string_view text, uint32_t duration=2000);
    void show(std::string_view text, uint32_t duration=2000);

    virtual void draw() override;
    virtual void tick(uint32_t tick) override;
};
