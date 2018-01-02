/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/stm32_flash/Storage.hpp>

namespace core {
namespace stm32_flash {
#ifndef CORE_PACKED
#define CORE_PACKED_ALIGNED  __attribute__((aligned(4), packed))
#endif

struct ModuleConfiguration {
    uint32_t imageCRC;
    uint32_t canID;
    char     name[16];
    uint8_t  padding[32 - sizeof(name) - sizeof(imageCRC) - sizeof(canID)];
}

CORE_PACKED_ALIGNED;


class ConfigurationStorage
{
public:
    ConfigurationStorage(
        Storage& storage
    );

    inline bool
    isReady() const;

    inline bool
    isUserAddressValid(
        Address address
    ) const;

    inline Address
    userFrom() const;

    inline Address
    userTo() const;

    const ModuleConfiguration*
    getModuleConfiguration() const;

    void*
    getUserConfiguration() const;

    bool
    writeModuleName(
        const char* name
    );

    bool
    writeProgramCRC(
        uint32_t crc
    );

    bool
    writeCanID(
        uint32_t id
    );

    bool
    erase();

    inline std::size_t
    userDataSize() const;

    bool
    writeUserData16(
        Address  offset,
        uint16_t data
    );

    bool
    writeUserData32(
        Address  offset,
        uint32_t data
    );

    bool
    beginWrite();

    bool
    endWrite();

    bool
    eraseUserConfiguration();

    bool
    lock();

    bool
    unlock();

    bool
    isValid() const;


private:
    Storage& _storage;
    bool     _ready;
};

// --------------------------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// --------------------------------------------------------------------------------------------------------------------

inline bool
ConfigurationStorage::isUserAddressValid(
    Address address
) const
{
    return (address >= userFrom()) && (address < userTo());
}

inline std::size_t
ConfigurationStorage::userDataSize() const
{
    return _storage.size() - sizeof(ModuleConfiguration);
}

inline Address
ConfigurationStorage::userFrom() const
{
    return 0;
}

inline Address
ConfigurationStorage::userTo() const
{
    return userDataSize();
}

inline bool
ConfigurationStorage::isReady() const
{
    return _ready;
}

inline bool
ConfigurationStorage::isValid() const
{
    return _storage.isValid();
}
}
}
