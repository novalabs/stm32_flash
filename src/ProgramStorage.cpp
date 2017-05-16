/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/stm32_flash/ProgramStorage.hpp>
#include <osal.h>

namespace core {
namespace stm32_flash {
ProgramStorage::ProgramStorage(
    FlashSegment& storage
) : _storage(storage), _ready(false) {}


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
