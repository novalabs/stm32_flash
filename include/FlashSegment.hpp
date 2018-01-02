/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <cstddef>
#include <stdint.h>

#include <core/stm32_flash/flash_segments.hpp>

namespace core {
namespace stm32_flash {
using Address = uint32_t;
using Sector  = std::size_t;

class FlashSegment
{
public:
    FlashSegment(
        uint32_t from,
        uint32_t to
    );
    ~FlashSegment();

    //--- CHECKS ------------------------------------------------------------------
    bool
    isAddressValid(
        Address address
    ) const;


    //--- LOCK --------------------------------------------------------------------
    bool
    lock();

    bool
    unlock();


    //--- ERASE -------------------------------------------------------------------
    bool
    erase();

    bool
    eraseSector(
        Sector sector
    );

    bool
    eraseSectorAt(
        Address address
    );

    bool
    eraseSectors(
        Sector from,
        Sector to
    );

    bool
    eraseSectorsAt(
        Address from,
        Address to
    );


    //--- READ & WRITE ------------------------------------------------------------
    uint32_t
    read32(
        Address address
    );

    uint32_t
    read32_offset(
        Address offset
    );

    uint16_t
    read16(
        Address address
    );

    uint16_t
    read16_offset(
        Address offset
    );

    bool
    write32(
        Address  address,
        uint32_t data
    );

    bool
    write32_offset(
        Address  offset,
        uint32_t data
    );

    bool
    write16(
        Address  address,
        uint16_t data
    );

    bool
    write16_offset(
        Address  offset,
        uint16_t data
    );


    inline Address
    from() const;

    inline Address
    to() const;

    inline std::size_t
    size() const;


private:
    const uint32_t _from;
    const uint32_t _to;
};

// --------------------------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// --------------------------------------------------------------------------------------------------------------------


inline
bool
FlashSegment::isAddressValid(
    Address address
) const
{
    if ((address < _from) || (address >= _to)) {
        return false;
    } else {
        return true;
    }
}

inline Address
FlashSegment::from() const
{
    return _from;
}

inline Address
FlashSegment::to() const
{
    return _to;
}

inline std::size_t
FlashSegment::size() const
{
    return _to - _from;
}

inline uint32_t
FlashSegment::read32(
    Address address
)
{
    return *(uint32_t*)(address);
}

inline uint32_t
FlashSegment::read32_offset(
    Address offset
)
{
    return *(uint32_t*)(offset + _from);
}

inline uint16_t
FlashSegment::read16(
    Address address
)
{
    return *(uint16_t*)(address);
}

inline uint16_t
FlashSegment::read16_offset(
    Address offset
)
{
    return *(uint16_t*)(offset + _from);
}
}
}
