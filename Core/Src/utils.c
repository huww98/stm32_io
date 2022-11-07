#include "utils.h"
#include "main.h"

void delay_us_impl(uint32_t us, uint32_t cycle_start) {
    uint32_t tick_start = HAL_GetTick();
    uint32_t cycles_per_tick = SysTick->LOAD;

    uint32_t cycles_per_us = SystemCoreClock / 1000000;
    uint32_t cycles = us * cycles_per_us;

    uint32_t tick_target = tick_start + cycles / cycles_per_tick;
    cycles %= cycles_per_tick;

    uint32_t cycle_target;
    if (cycles > cycle_start) {
        tick_target++;
        cycle_target = cycle_start + cycles_per_tick - cycles;
    } else {
        cycle_target = cycle_start - cycles;
    }

    while (tick_target > HAL_GetTick()) {
        __WFI();
    }
    while (tick_target == HAL_GetTick() && cycle_target < SysTick->VAL)
        ;
}
