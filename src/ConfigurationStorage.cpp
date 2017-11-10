/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/common.hpp>
#include <core/stm32_flash/ConfigurationStorage.hpp>

namespace core {
namespace stm32_flash {
ConfigurationStorage::ConfigurationStorage(
    Storage& storage
) : _storage(storage), _ready(false) {}

const ModuleConfiguration*
ConfigurationStorage::getModuleConfiguration() const
{
    static const ModuleConfiguration defaultConfiguration = {
        0, 0xFFFFFFFF, "***invalid***", 0
    };

    if (_storage.isValid()) {
        return reinterpret_cast<ModuleConfiguration*>(_storage.getAddress());
    } else {
        return &defaultConfiguration;
    }
}

void*
ConfigurationStorage::getUserConfiguration() const
{
    if (_storage.isValid()) {
        return reinterpret_cast<void*>(_storage.getAddress() + sizeof(ModuleConfiguration));
    } else {
        return nullptr;
    }
}

bool
ConfigurationStorage::writeModuleName(
    const char* name
)
{
    const uint16_t* tmp = nullptr;
    bool success = true;

    success &= _storage.format();

    if (success) {
    	CORE_WARNINGS_NO_CAST_ALIGN
        tmp = reinterpret_cast<const uint16_t*>(name); //NOTE: warning: cast increases required alignment of target type - It is OK by design
    	CORE_WARNINGS_RESET

        for (std::size_t i = 0; i < sizeof(ModuleConfiguration::name); i += 2) {
            success &= _storage.write16(offsetof(ModuleConfiguration, name) + i, *tmp++);
        }

        success &= _storage.write32(offsetof(ModuleConfiguration, imageCRC), getModuleConfiguration()->imageCRC);
        success &= _storage.write32(offsetof(ModuleConfiguration, canID), getModuleConfiguration()->canID);

        tmp = reinterpret_cast<const uint16_t*>(getUserConfiguration());

        if (tmp != nullptr) {
            for (std::size_t i = 0; i < _storage.size() - sizeof(ModuleConfiguration); i += 2) {
                success &= _storage.write16(sizeof(ModuleConfiguration) + i, *tmp++);
            }
        }

        success &= _storage.commit();
    }

    return success;
} // writeModuleName

bool
ConfigurationStorage::writeProgramCRC(
    uint32_t crc
)
{
    const uint16_t* tmp = nullptr;
    bool success = true;

    success &= _storage.format();

    if (success) {
    	CORE_WARNINGS_NO_CAST_ALIGN
        tmp = reinterpret_cast<const uint16_t*>(getModuleConfiguration()->name); //NOTE: warning: cast increases required alignment of target type - It is OK by design
    	CORE_WARNINGS_RESET

        for (std::size_t i = 0; i < sizeof(ModuleConfiguration::name); i += 2) {
            success &= _storage.write16(offsetof(ModuleConfiguration, name) + i, *tmp++);
        }

        success &= _storage.write32(offsetof(ModuleConfiguration, imageCRC), crc);
        success &= _storage.write32(offsetof(ModuleConfiguration, canID), getModuleConfiguration()->canID);

        tmp = reinterpret_cast<const uint16_t*>(getUserConfiguration());

        if (tmp != nullptr) {
            for (std::size_t i = 0; i < _storage.size() - sizeof(ModuleConfiguration); i += 2) {
                success &= _storage.write16(sizeof(ModuleConfiguration) + i, *tmp++);
            }
        }

        success &= _storage.commit();
    }

    return success;
} // writeProgramCRC

bool
ConfigurationStorage::writeCanID(
    uint32_t id
)
{
    const uint16_t* tmp = nullptr;
    bool success = true;

    success &= _storage.format();

    if (success) {
    	CORE_WARNINGS_NO_CAST_ALIGN
        tmp = reinterpret_cast<const uint16_t*>(getModuleConfiguration()->name); //NOTE: warning: cast increases required alignment of target type - It is OK by design
    	CORE_WARNINGS_RESET

        for (std::size_t i = 0; i < sizeof(ModuleConfiguration::name); i += 2) {
            success &= _storage.write16(offsetof(ModuleConfiguration, name) + i, *tmp++);
        }

        success &= _storage.write32(offsetof(ModuleConfiguration, imageCRC), getModuleConfiguration()->imageCRC);
        success &= _storage.write32(offsetof(ModuleConfiguration, canID), id);

        tmp = reinterpret_cast<const uint16_t*>(getUserConfiguration());

        if (tmp != nullptr) {
            for (std::size_t i = 0; i < _storage.size() - sizeof(ModuleConfiguration); i += 2) {
                success &= _storage.write16(sizeof(ModuleConfiguration) + i, *tmp++);
            }
        }

        success &= _storage.commit();
    }

    return success;
} // writeProgramCRC

bool
ConfigurationStorage::erase()
{
    return _storage.erase();
}

bool
ConfigurationStorage::writeUserData16(
    Address  offset,
    uint16_t data
)
{
    if (_ready) {
        return _storage.write16(offset + sizeof(ModuleConfiguration), data);
    } else {
        return false;
    }
}

bool
ConfigurationStorage::writeUserData32(
    Address  offset,
    uint32_t data
)
{
    if (_ready) {
        return _storage.write32(offset + sizeof(ModuleConfiguration), data);
    } else {
        return false;
    }
}

bool
ConfigurationStorage::beginWrite()
{
    const uint16_t* tmp = nullptr;
    bool success = true;

    success &= unlock();
    success &= _storage.format();

    if (success) {
    	CORE_WARNINGS_NO_CAST_ALIGN
        tmp = reinterpret_cast<const uint16_t*>(getModuleConfiguration()->name); //NOTE: warning: cast increases required alignment of target type - It is OK by design
    	CORE_WARNINGS_RESET

        for (std::size_t i = 0; i < sizeof(ModuleConfiguration::name); i += 2) {
            success &= _storage.write16(offsetof(ModuleConfiguration, name) + i, *tmp++);
        }

        success &= _storage.write32(offsetof(ModuleConfiguration, imageCRC), getModuleConfiguration()->imageCRC);
        success &= _storage.write32(offsetof(ModuleConfiguration, canID), getModuleConfiguration()->canID);
    }

    _ready = success;

    return success;
} // beginWrite

bool
ConfigurationStorage::endWrite()
{
    bool success = true;

    success &= _storage.commit();
    success &= lock();
    _ready   = false;

    return success;
}

bool
ConfigurationStorage::eraseUserConfiguration()
{
    const uint16_t* tmp = nullptr;
    bool success = true;

    success &= _storage.format();

    if (success) {
    	CORE_WARNINGS_NO_CAST_ALIGN
        tmp = reinterpret_cast<const uint16_t*>(getModuleConfiguration()->name); //NOTE: warning: cast increases required alignment of target type - It is OK by design
    	CORE_WARNINGS_RESET

        for (std::size_t i = 0; i < sizeof(ModuleConfiguration::name); i += 2) {
            success &= _storage.write16(offsetof(ModuleConfiguration, name) + i, *tmp++);
        }

        success &= _storage.write32(offsetof(ModuleConfiguration, imageCRC), getModuleConfiguration()->imageCRC);
        success &= _storage.write32(offsetof(ModuleConfiguration, canID), getModuleConfiguration()->canID);

        success &= _storage.commit();
    }

    return success;
} // eraseUserConfiguration

bool
ConfigurationStorage::lock()
{
    return _storage.lock();
}

bool
ConfigurationStorage::unlock()
{
    return _storage.unlock();
}
}
}
