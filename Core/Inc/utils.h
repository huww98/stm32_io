#pragma once

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

void delay_us_impl(uint32_t us, uint32_t cycle_start);

void inline delay_us(uint32_t us) {
  uint32_t cycle_start = SysTick->VAL;
  delay_us_impl(us, cycle_start);
}

#ifdef __cplusplus
}
#endif
