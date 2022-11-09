#pragma once

#include <array>
#include <cassert>

#include "button.h"

class ui_base {
  public:
    virtual void handle_button(uint8_t button, button_event event){};
    virtual void draw() = 0;
};

class page_manager {
  private:
    ui_base *_current_page;
    std::array<ui_base *, 8> page_stack;
    uint8_t stack_ptr = 0;

  public:
    page_manager() : _current_page() {}

    void init(ui_base &page) {
        assert(_current_page == nullptr);
        _current_page = &page;
        _current_page->draw();
    }

    void push(ui_base &page) {
        assert(stack_ptr < page_stack.size());
        page_stack[stack_ptr++] = _current_page;
        _current_page = &page;
        _current_page->draw();
    }

    void pop() {
        assert(stack_ptr > 0);
        _current_page = page_stack[--stack_ptr];
        _current_page->draw();
    }

    ui_base &current_page() const { return *_current_page; }
};

extern page_manager pm;
