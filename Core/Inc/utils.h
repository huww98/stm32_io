#include "main.h"

void delay_us_impl(uint32_t us, uint32_t cycle_start);

void inline __attribute__((always_inline)) delay_us(uint32_t us) {
  uint32_t cycle_start = SysTick->VAL;
  delay_us_impl(us, cycle_start);
}

