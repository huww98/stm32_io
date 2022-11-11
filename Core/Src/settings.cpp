#include "settings.h"
#include "main.h"

#include <cassert>

constexpr uint16_t SETTINGS_MAGIC = 0x5A6A;
constexpr uint32_t SETTINGS_FLASH_ADDR = 0x0800fc00;

void settings_t::save() {
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef erase {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks = FLASH_BANK_1,
        .PageAddress = SETTINGS_FLASH_ADDR,
        .NbPages = 1,
    };
    uint32_t page_error;
    HAL_FLASHEx_Erase(&erase, &page_error);
    assert(page_error == 0xffffffff);

    auto next_config_addr = SETTINGS_FLASH_ADDR;
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, next_config_addr, SETTINGS_MAGIC);
    next_config_addr += 2;

    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, next_config_addr, contrast);
    next_config_addr += 2;

    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, next_config_addr, sleep_timeout);
    next_config_addr += 2;

    assert(next_config_addr <= SETTINGS_FLASH_ADDR + 0x400);
    HAL_FLASH_Lock();
}

void settings_t::load() {
    if (*(uint16_t *)SETTINGS_FLASH_ADDR != SETTINGS_MAGIC)
        return;
    auto next_config_addr = SETTINGS_FLASH_ADDR + 2;
    contrast = *(uint16_t *)next_config_addr;
    next_config_addr += 2;
    sleep_timeout = *(uint16_t *)next_config_addr;
    next_config_addr += 2;
    assert(next_config_addr <= SETTINGS_FLASH_ADDR + 0x400);
}

void settings_t::reset() {
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef erase {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks = FLASH_BANK_1,
        .PageAddress = SETTINGS_FLASH_ADDR,
        .NbPages = 1,
    };
    uint32_t page_error;
    HAL_FLASHEx_Erase(&erase, &page_error);
    assert(page_error == 0xffffffff);
    HAL_FLASH_Lock();
}
