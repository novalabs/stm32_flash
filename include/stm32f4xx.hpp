/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include "stm32f4xx.h"

#include <stdint.h>
#include <cstddef>

#if defined(STM32F407VE)
#define FLASH_SIZE 512
#elif defined(STM32F407VG)
#define FLASH_SIZE 1024
#endif

namespace core {
namespace stm32_flash {
#if defined(STM32F407VE)
static const std::size_t FLASH_NUMBER_OF_PAGES = 8;
#elif defined(STM32F407VG)
static const std::size_t FLASH_NUMBER_OF_PAGES = 12;
#else
#error "Unknown flash memory map"
#endif

static constexpr uint32_t
FLASH_SECTOR_SIZE(
    std::size_t sector
)
{
#if defined(STM32F407VE) or defined(STM32F407VG)
    return sector < 4 ? 0x4000 :
		   sector == 4 ? 0x10000 :
		   sector < FLASH_NUMBER_OF_PAGES ? 0x20000 :
		   0xFFFFFFFF;

#else
#error "Unknown flash memory map"
#endif
}

static constexpr uint32_t
FLASH_SECTOR_OFFSET(
    std::size_t sector
)
{
#if defined(STM32F407VE) or defined(STM32F407VG)
	return sector == 0 ? 0 :
		   sector < FLASH_NUMBER_OF_PAGES ? FLASH_SECTOR_OFFSET(sector - 1) + FLASH_SECTOR_SIZE(sector):
		   0xFFFFFFFF;
#else
#error "Unknown flash memory map"
#endif
}

static constexpr uint32_t
FLASH_SECTOR_ADDRESS(
    std::size_t sector
)
{
#if defined(STM32F407VE) or defined(STM32F407VG)
    return sector < FLASH_NUMBER_OF_PAGES ? 0x08000000 + FLASH_SECTOR_OFFSET(sector) : 0xFFFFFFFF;

#else
#error "Unknown flash memory map"
#endif
}

static constexpr uint32_t
FLASH_ADDRESS_SECTOR(
    uint32_t address
)
{
#define X(N) (address >= FLASH_SECTOR_ADDRESS(N) && address < FLASH_SECTOR_ADDRESS(N+1)) ? N :
#if defined(STM32F407VE)
    return X(0) X(1) X(2) X(3) X(4) X(5) X(6) X(7) 0xFFFFFFFF;
#elif defined(STM32F407VG)
    return X(0) X(1) X(2) X(3) X(4) X(5) X(6) X(7) X(8) X(9) X(10) X(11) 0xFFFFFFFF;
#else
#error "Unknown flash memory map"
#endif
}
}
}
