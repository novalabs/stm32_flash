/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/stm32_flash/FlashSegment.hpp>

#include <cstddef>
#include <stdint.h>

namespace core {
namespace stm32_flash {
class Storage
{
public:
    Storage(
        FlashSegment& bank1,
        FlashSegment& bank2
    );

    bool
    isValid() const;

    inline
    operator void*() const;

    inline Address
    getAddress() const;

    bool
    erase();

    bool
    format();

    bool
    isWriteReady() const;

    bool
    commit();

    inline bool
    write16(
        Address  offset,
        uint16_t data
    );

    inline bool
    write32(
        Address  offset,
        uint32_t data
    );

    inline std::size_t
    size() const;

    bool
    lock();

    bool
    unlock();


private:
    FlashSegment& _bank1;
    FlashSegment& _bank2;
    uint16_t      _cnt;
    FlashSegment* _readBank;
    FlashSegment* _writeBank;
    uint32_t      _head;
    bool          _writeReady;
    std::size_t   _bankSize;
};

// --------------------------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// --------------------------------------------------------------------------------------------------------------------

Storage::operator void*() const
{
    return reinterpret_cast<void*>(getAddress());
}

Address
Storage::getAddress() const
{
    return _readBank->from() + 4;
}

bool
Storage::write16(
    Address  offset,
    uint16_t data
)
{
    return _writeBank->write16_offset(offset + 4, data);
}

bool
Storage::write32(
    Address  offset,
    uint32_t data
)
{
    return _writeBank->write32_offset(offset + 4, data);
}

std::size_t
Storage::size() const
{
    return _bankSize - 4;
}
}
}
