/*
 * stm32f30x.hpp
 *
 *  Created on: 9 May 2016
 *      Author: davide
 */

#ifndef STM32F30X_HPP_
#define STM32F30X_HPP_

#include "stm32f30x.h"

#include <stdint.h>
#include <cstddef>

namespace core {
namespace stm32_flash {
#ifdef STM32F303xC
static const std::size_t FLASH_NUMBER_OF_PAGES = 128;
#else
#error "Unknown flash memory map"
#endif

static constexpr uint32_t
FLASH_SECTOR_SIZE(
    std::size_t sector
)
{
#ifdef STM32F303xC
    return sector < FLASH_NUMBER_OF_PAGES ? 0x800 : 0xFFFFFFFF;

#else
#error "Unknown flash memory map"
#endif
}

static constexpr uint32_t
FLASH_SECTOR_OFFSET(
    std::size_t sector
)
{
#ifdef STM32F303xC
    return sector < FLASH_NUMBER_OF_PAGES ? (0x800 * sector) : 0xFFFFFFFF;

#else
#error "Unknown flash memory map"
#endif
}

static constexpr uint32_t
FLASH_SECTOR_ADDRESS(
    std::size_t sector
)
{
#ifdef STM32F303xC
    return sector < FLASH_NUMBER_OF_PAGES ? 0x08000000 + (0x800 * sector) : 0xFFFFFFFF;

#else
#error "Unknown flash memory map"
#endif
}

static constexpr uint32_t
FLASH_ADDRESS_SECTOR(
    uint32_t address
)
{
#ifdef STM32F303xC
    return (address - 0x08000000) >> 11;

#else
#error "Unknown flash memory map"
#endif
}
}
}

#endif /* STM32F30X_HPP_ */
