/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
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
#elif defined(STM32F407xx) || defined(STM32F417xx)
    #include <core/stm32_flash/stm32f4xx_flash.h>
    #include <core/stm32_flash/stm32f4xx.hpp>
FLASH_Status FLASH_ErasePage(uint32_t Page_Address) {
	switch(Page_Address) {
		case core::stm32_flash::FLASH_SECTOR_ADDRESS(0): FLASH_EraseSector(FLASH_Sector_0, VoltageRange_3); break;
		case core::stm32_flash::FLASH_SECTOR_ADDRESS(1): FLASH_EraseSector(FLASH_Sector_1, VoltageRange_3); break;
		case core::stm32_flash::FLASH_SECTOR_ADDRESS(2): FLASH_EraseSector(FLASH_Sector_2, VoltageRange_3); break;
		case core::stm32_flash::FLASH_SECTOR_ADDRESS(3): FLASH_EraseSector(FLASH_Sector_3, VoltageRange_3); break;
		case core::stm32_flash::FLASH_SECTOR_ADDRESS(4): FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3); break;
		case core::stm32_flash::FLASH_SECTOR_ADDRESS(5): FLASH_EraseSector(FLASH_Sector_5, VoltageRange_3); break;
		case core::stm32_flash::FLASH_SECTOR_ADDRESS(6): FLASH_EraseSector(FLASH_Sector_6, VoltageRange_3); break;
		case core::stm32_flash::FLASH_SECTOR_ADDRESS(7): FLASH_EraseSector(FLASH_Sector_7, VoltageRange_3); break;
#if defined(STM32F407VG) || defined(STM32F417VG)
		case core::stm32_flash::FLASH_SECTOR_ADDRESS(8): FLASH_EraseSector(FLASH_Sector_8, VoltageRange_3); break;
		case core::stm32_flash::FLASH_SECTOR_ADDRESS(9): FLASH_EraseSector(FLASH_Sector_9, VoltageRange_3); break;
		case core::stm32_flash::FLASH_SECTOR_ADDRESS(10): FLASH_EraseSector(FLASH_Sector_10, VoltageRange_3); break;
		case core::stm32_flash::FLASH_SECTOR_ADDRESS(11): FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3); break;
#endif
		default:
			return FLASH_ERROR_OPERATION;
	}
	return FLASH_COMPLETE;
}
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
	volatile Address address2 = FLASH_SECTOR_ADDRESS(FLASH_ADDRESS_SECTOR(address)); // Offset 0 from sector start address!

    if (!isAddressValid(address2)) {
        return false;
    }

    //__disable_irq();
    bool success = FLASH_ErasePage(address2) == FLASH_COMPLETE;
    //__enable_irq();


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

    //__disable_irq();
    bool success = FLASH_ProgramWord(address, data) == FLASH_COMPLETE;
    //__enable_irq();

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

    //__disable_irq();
    bool success = FLASH_ProgramWord(address, data) == FLASH_COMPLETE;
    //__enable_irq();

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

    //__disable_irq();
    bool success = FLASH_ProgramHalfWord(address, data) == FLASH_COMPLETE;
    //__enable_irq();

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

    //__disable_irq();
    bool success = FLASH_ProgramHalfWord(address, data) == FLASH_COMPLETE;
    //__enable_irq();

    return success;
}
}
}
