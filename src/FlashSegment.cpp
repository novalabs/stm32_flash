/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/stm32_flash/FlashSegment.hpp>
#include <osal.h>

#if defined(STM32F303xx)
    #include <core/stm32_flash/stm32f30x_flash.h>
    #include <core/stm32_flash/stm32f30x.hpp>
#elif defined(STM32F091xC)
    #include <core/stm32_flash/stm32f0xx_flash.h>
    #include <core/stm32_flash/stm32f0xx.hpp>
#elif defined(STM32F407xx)
	#include<core/stm32_flash/stm32f4xx_flash.h>
    #include <core/stm32_flash/stm32f4xx.hpp>
    #define FLASH_ErasePage(x) FLASH_EraseSector(x, VoltageRange_3)
#else
    #error "Chip not supported"
#endif

namespace core {
namespace stm32_flash {
FlashSegment::FlashSegment(
    uint32_t from,
    uint32_t to
) :
    _from(from),
    _to(to)
{}

FlashSegment::~FlashSegment() {}

bool
FlashSegment::lock()
{
    FLASH_Lock();

    return true;
}

bool
FlashSegment::unlock()
{
    FLASH_Unlock();
    return true;
}

bool
FlashSegment::erase()
{
    return FlashSegment::eraseSectorsAt(_from, _to);
}

bool
FlashSegment::eraseSector(
    Sector sector
)
{
    uint32_t address = FLASH_SECTOR_ADDRESS(sector);

    return eraseSectorAt(address);
}

bool
FlashSegment::eraseSectorAt(
    Address address
)
{
    address = FLASH_SECTOR_ADDRESS(FLASH_ADDRESS_SECTOR(address)); // Offset 0 from sector start address!

    if (!isAddressValid(address)) {
        return false;
    }

    __disable_irq();
    bool success = FLASH_ErasePage(address) == FLASH_COMPLETE;
    __enable_irq();


    return success;
}

bool
FlashSegment::eraseSectors(
    Sector from,
    Sector to
)
{
    if (from > to) {
        return false;
    }

    for (std::size_t i = from; i < to; i++) {
        if (!eraseSector(i)) {
            return false;
        }
    }

    return true;
}

bool
FlashSegment::eraseSectorsAt(
    Address from,
    Address to
)
{
    if (from > to) {
        return false;
    }

    Sector start = FLASH_ADDRESS_SECTOR(from);
    Sector end   = FLASH_ADDRESS_SECTOR(to);

    return eraseSectors(start, end);
}

bool
FlashSegment::write32(
    Address  address,
    uint32_t data
)
{
    if (!isAddressValid(address)) {
        return false;
    }

    __disable_irq();
    bool success = FLASH_ProgramWord(address, data) == FLASH_COMPLETE;
    __enable_irq();

    return success;
}

bool
FlashSegment::write32_offset(
    Address  offset,
    uint32_t data
)
{
    Address address = _from + offset;

    if (!isAddressValid(address)) {
        return false;
    }

    __disable_irq();
    bool success = FLASH_ProgramWord(address, data) == FLASH_COMPLETE;
    __enable_irq();

    return success;
}

bool
FlashSegment::write16(
    Address  address,
    uint16_t data
)
{
    if (!isAddressValid(address)) {
        return false;
    }

    __disable_irq();
    bool success = FLASH_ProgramHalfWord(address, data) == FLASH_COMPLETE;
    __enable_irq();

    return success;
}

bool
FlashSegment::write16_offset(
    Address  offset,
    uint16_t data
)
{
    Address address = _from + offset;

    if (!isAddressValid(address)) {
        return false;
    }

    __disable_irq();
    bool success = FLASH_ProgramHalfWord(address, data) == FLASH_COMPLETE;
    __enable_irq();

    return success;
}
}
}
