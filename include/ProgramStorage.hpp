/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/stm32_flash/FlashSegment.hpp>

namespace core {
namespace stm32_flash {
class ProgramStorage
{
public:
    ProgramStorage(
        FlashSegment& storage
    );

    inline bool
    isReady() const;

    inline bool
    isAddressValid(
        Address address
    ) const;

    inline Address
    from() const;

    inline Address
    to() const;

    bool
    erase();

    inline std::size_t
    size() const;

    inline bool
    write16(
        Address  address,
        uint16_t data
    );

    bool
    beginWrite();

    bool
    endWrite();

    bool
    lock();

    bool
    unlock();

    uint32_t updateCRC();
    uint32_t crc();

private:
    FlashSegment& _storage;
    bool          _ready;
    uint32_t      _crc;
};

// --------------------------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// --------------------------------------------------------------------------------------------------------------------

inline bool
ProgramStorage::isAddressValid(
    Address address
) const
{
    return (address >= from()) && (address < to());
}

inline bool
ProgramStorage::isReady() const
{
    return _ready;
}

inline Address
ProgramStorage::from() const
{
    return _storage.from();
}

inline Address
ProgramStorage::to() const
{
    return _storage.to();
}

inline std::size_t
ProgramStorage::size() const
{
    return _storage.size();
}

bool
ProgramStorage::write16(
    Address  address,
    uint16_t data
)
{
    if (_ready) {
        return _storage.write16(address, data);
    } else {
        return false;
    }
}

inline uint32_t
ProgramStorage::crc() {
    return _crc;
}
}
}
