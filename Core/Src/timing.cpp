#include "timing.h"
#include "main.h"

#include <cassert>

constexpr uint32_t TIMING_FLASH_ADDR = 0x0800f800;
constexpr uint16_t TIMING_MAGIC = 0x1234;

void timing_t::save() {
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef erase {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks = FLASH_BANK_1,
        .PageAddress = TIMING_FLASH_ADDR,
        .NbPages = 1,
    };
    uint32_t page_error;
    HAL_FLASHEx_Erase(&erase, &page_error);
    assert(page_error == 0xffffffff);

    auto next_config_addr = TIMING_FLASH_ADDR;
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, next_config_addr, TIMING_MAGIC);
    next_config_addr += 2;

    for (int i = 0; i < 24; i++) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, next_config_addr, shutter_delay[i]);
        next_config_addr += 2;
    }
    uint32_t enabled_packed = 0;
    for (int i = 0; i < 24; i++) {
        enabled_packed |= (uint32_t)enabled[i] << i;
    }
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, next_config_addr, enabled_packed);
    next_config_addr += 4;

    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, next_config_addr, base_delay);
    next_config_addr += 2;

    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, next_config_addr, focus_advance);
    next_config_addr += 2;

    assert(next_config_addr <= TIMING_FLASH_ADDR + 0x400);
    HAL_FLASH_Lock();

    dirty = false;
}

void timing_t::load() {
    if (*(uint16_t *)TIMING_FLASH_ADDR != TIMING_MAGIC)
        return;
    auto next_config_addr = TIMING_FLASH_ADDR + 2;
    for (int i = 0; i < 24; i++) {
        shutter_delay[i] = *(uint16_t *)next_config_addr;
        next_config_addr += 2;
    }
    uint32_t enabled_packed = *(uint32_t *)next_config_addr;
    for (int i = 0; i < 24; i++) {
        enabled[i] = enabled_packed & (1 << i);
    }
    next_config_addr += 4;

    base_delay = *(uint16_t *)next_config_addr;
    next_config_addr += 2;

    focus_advance = *(uint16_t *)next_config_addr;
    next_config_addr += 2;

    dirty = false;
}
