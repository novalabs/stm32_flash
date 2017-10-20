/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <osal.h>
#include <core/stm32_crc/CRC.hpp>
#include <core/stm32_flash/ProgramStorage.hpp>

namespace core {
namespace stm32_flash {
ProgramStorage::ProgramStorage(
    FlashSegment& storage
) : _storage(storage), _ready(false), _crc(0) {
}

uint32_t
ProgramStorage::updateCRC() {
    core::stm32_crc::CRC::init();
    core::stm32_crc::CRC::setPolynomialSize(core::stm32_crc::CRC::PolynomialSize::POLY_32);
    core::stm32_crc::CRC::CRCBlock((uint32_t*)from(), size() / sizeof(uint32_t));

    _crc =  core::stm32_crc::CRC::getCRC();

    return _crc;
}

bool
ProgramStorage::erase()
{
    return _storage.erase();
}

bool
ProgramStorage::beginWrite()
{
    bool success = true;

    success &= unlock();
    success &= erase();

    _ready = success;

    return success;
}

bool
ProgramStorage::endWrite()
{
    bool success = true;

    success &= lock();
    _ready   = false;

    return success;
}

bool
ProgramStorage::lock()
{
    return _storage.lock();
}

bool
ProgramStorage::unlock()
{
    return _storage.unlock();
}
}
}
