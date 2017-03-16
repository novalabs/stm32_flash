/*
 * hw_flash.hpp
 *
 *  Created on: 6 May 2016
 *      Author: davide
 */

#ifndef HW_FLASH_HPP_
#define HW_FLASH_HPP_

#include <cstddef>
#include <stdint.h>
#include <algorithm>
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
    );


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
    from()
    {
        return _from;
    }

    inline Address
    to()
    {
        return _to;
    }

    inline std::size_t
    size()
    {
        return _to - _from;
    }

private:
    const uint32_t _from;
    const uint32_t _to;
};

// --------------------------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// --------------------------------------------------------------------------------------------------------------------

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

class Storage
{
public:
    Storage(
        FlashSegment& bank1,
        FlashSegment& bank2
    ) : _bank1(bank1), _bank2(bank2), _cnt(0xFFFF), _readBank(nullptr), _writeBank(nullptr), _head(0), _writeReady(false), _bankSize(0)
    {
        uint16_t cnt1       = _bank1.read16_offset(0);
        uint16_t cnt2       = _bank2.read16_offset(0);
        bool     bank1Valid = false;
        bool     bank2Valid = false;

        if (cnt1 != 0xFFFF) {
            bank1Valid = true;
        }

        if (cnt2 != 0xFFFF) {
            bank2Valid = true;
        }

        if (bank1Valid && bank2Valid) {
            // Both banks are valid, choose the newest
            if (cnt1 > cnt2) {
                _cnt       = cnt1;
                _head      = _bank1.from() + 4;
                _readBank  = &_bank1;
                _writeBank = &_bank2;
                //_bankSize = _bank1.size();
            } else if (cnt2 > cnt1) {
                _cnt       = cnt2;
                _head      = _bank2.from() + 4;
                _readBank  = &_bank2;
                _writeBank = &_bank1;
                //_bankSize = _bank2.size();
            } else {
                // This case is not possible, but handle it anyway...
                bank1Valid = false;
                bank2Valid = false;
            }
        } else {
            if (bank1Valid) {
                // only bank1 is valid, lets erase bank2, and set it as write bank
                _cnt       = cnt1;
                _readBank  = &_bank1;
                _writeBank = &_bank2;
            } else if (bank2Valid) {
                // only bank2 is valid, lets erase bank1, and set it as write bank
                _cnt       = cnt2;
                _readBank  = &_bank2;
                _writeBank = &_bank1;
            } else {
                // We have no valid read bank.
                // See below
            }
        }

        if (!(bank1Valid || bank2Valid)) {
            // We have no valid read bank.
            // set bank1  as write bank
            _cnt       = 0xFFFF;
            _writeBank = &_bank1;
        }

        _bankSize = std::min(_bank1.size(), _bank2.size());
    }

    bool
    isValid() const
    {
        return _readBank != nullptr;
    }

    operator void*() const {
        return reinterpret_cast<void*>(getAddress());
    }

    Address
    getAddress() const
    {
        return _readBank->from() + 4;
    }

    bool
    erase()
    {
        bool success = true;

        success   &= _bank1.erase();
        success   &= _bank2.erase();
        _cnt       = 0xFFFF;
        _readBank  = nullptr;
        _writeBank = &_bank1;

        return success;
    }

    bool
    format()
    {
        // write bank is always defined
        _writeBank->unlock();
        _writeReady = _writeBank->erase();
        return _writeReady;
    }

    bool
    isWriteReady() const
    {
        return _writeReady;
    }

    bool
    commit()
    {
        if (!_writeReady) {
            return false;
        }

        _writeReady = false;

        _cnt++;

        if (_cnt == 0xFFFF) {
            _cnt = 0;
        }

        // write bank is always defined
        bool success = true;
        success &= _writeBank->write16_offset(0, _cnt);

        _writeBank->lock();
        // Swap the 2 banks
        FlashSegment* tmp = _readBank;
        _readBank  = _writeBank;
        _writeBank = tmp;

        return success;
    } // commit

    bool
    write16(
        Address  offset,
        uint16_t data
    )
    {
        return _writeBank->write16_offset(offset + 4, data);
    }

    bool
    write32(
        Address  offset,
        uint32_t data
    )
    {
        return _writeBank->write32_offset(offset + 4, data);
    }

    std::size_t
    size() const
    {
        return _bankSize - 4;
    }

    bool
    lock()
    {
        return _writeBank->lock();
    }

    bool
    unlock()
    {
        return _writeBank->unlock();
    }

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

struct ModuleConfiguration {
    char     name[14];
    uint32_t imageCRC;
    uint8_t  crap[32 - sizeof(name) - sizeof(imageCRC)];
};

class ProgramStorage
{
public:
    ProgramStorage(
        FlashSegment& storage
    ) : _storage(storage), _ready(false) {}

    inline bool
    isReady()
    {
        return _ready;
    }

    inline bool
    isAddressValid(
        Address address
    )
    {
        return (address >= from()) && (address < to());
    }

    inline Address
    from()
    {
        return _storage.from();
    }

    inline Address
    to()
    {
        return _storage.to();
    }

    bool
    erase()
    {
        return _storage.erase();
    }

    inline std::size_t
    size()
    {
        return _storage.size();
    }

    inline bool
    write16(
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

    bool
    beginWrite()
    {
        bool success = true;

        success &= unlock();
        success &= erase();

        _ready = success;

        return success;
    }

    bool
    endWrite()
    {
        bool success = true;

        success &= lock();
        _ready   = false;

        return success;
    }

    bool
    lock()
    {
        return _storage.lock();
    }

    bool
    unlock()
    {
        return _storage.unlock();
    }

private:
    FlashSegment& _storage;
    bool          _ready;
};

class ConfigurationStorage
{
public:
    ConfigurationStorage(
        Storage& storage
    ) : _storage(storage), _ready(false) {}

    inline bool
    isReady()
    {
        return _ready;
    }

    inline bool
    isUserAddressValid(
        Address address
    )
    {
        return (address >= userFrom()) && (address < userTo());
    }

    inline Address
    userFrom()
    {
        return 0;
    }

    inline Address
    userTo()
    {
        return userDataSize();
    }

    const ModuleConfiguration*
    getModuleConfiguration() const
    {
        static const ModuleConfiguration defaultConfiguration = {
            0
        };

        if (_storage.isValid()) {
            return reinterpret_cast<ModuleConfiguration*>(_storage.getAddress());
        } else {
            return &defaultConfiguration;
        }
    }

    void*
    getUserConfiguration() const
    {
        if (_storage.isValid()) {
            return reinterpret_cast<void*>(_storage.getAddress() + sizeof(ModuleConfiguration));
        } else {
            return nullptr;
        }
    }

    bool
    writeModuleName(
        const char* name
    )
    {
        const uint16_t* tmp = nullptr;
        bool success = true;

        _storage.format();

        tmp = reinterpret_cast<const uint16_t*>(name);

        for (std::size_t i = 0; i < sizeof(ModuleConfiguration::name); i += 2) {
            success &= _storage.write16(offsetof(ModuleConfiguration, name) + i, *tmp++);
        }

        success &= _storage.write32(offsetof(ModuleConfiguration, imageCRC), getModuleConfiguration()->imageCRC);

        tmp = reinterpret_cast<const uint16_t*>(getUserConfiguration());

        if (tmp != nullptr) {
            for (std::size_t i = 0; i < _storage.size() - sizeof(ModuleConfiguration); i += 2) {
                success &= _storage.write16(sizeof(ModuleConfiguration) + i, *tmp++);
            }
        }

        _storage.commit();

        return success;
    } // writeModuleName

    bool
    writeProgramCRC(
        uint32_t crc
    )
    {
        const uint16_t* tmp = nullptr;
        bool success = true;

        _storage.format();

        tmp = reinterpret_cast<const uint16_t*>(getModuleConfiguration()->name);

        for (std::size_t i = 0; i < sizeof(ModuleConfiguration::name); i += 2) {
            success &= _storage.write16(offsetof(ModuleConfiguration, name) + i, *tmp++);
        }

        success &= _storage.write32(offsetof(ModuleConfiguration, imageCRC), crc);

        tmp = reinterpret_cast<const uint16_t*>(getUserConfiguration());

        if (tmp != nullptr) {
            for (std::size_t i = 0; i < _storage.size() - sizeof(ModuleConfiguration); i += 2) {
                success &= _storage.write16(sizeof(ModuleConfiguration) + i, *tmp++);
            }
        }

        _storage.commit();

        return success;
    } // writeProgramCRC

    bool
    erase()
    {
        return _storage.erase();
    }

    inline std::size_t
    userDataSize()
    {
        return _storage.size() - sizeof(ModuleConfiguration);
    }

    inline bool
    writeUserData16(
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
    beginWrite()
    {
        const uint16_t* tmp = nullptr;
        bool success = true;

        success &= unlock();
        success &= _storage.format();

        tmp = reinterpret_cast<const uint16_t*>(getModuleConfiguration()->name);

        for (std::size_t i = 0; i < sizeof(ModuleConfiguration::name); i += 2) {
            success &= _storage.write16(offsetof(ModuleConfiguration, name) + i, *tmp++);
        }

        success &= _storage.write32(offsetof(ModuleConfiguration, imageCRC), getModuleConfiguration()->imageCRC);

        _ready = success;
        return success;
    } // beginWrite

    bool
    endWrite()
    {
        bool success = true;

        success &= _storage.commit();
        success &= lock();
        _ready   = false;

        return success;
    }

    bool
    eraseUserConfiguration()
    {
        const uint16_t* tmp = nullptr;
        bool success = true;

        success &= _storage.format();

        tmp = reinterpret_cast<const uint16_t*>(getModuleConfiguration()->name);

        for (std::size_t i = 0; i < sizeof(ModuleConfiguration::name); i += 2) {
            success &= _storage.write16(offsetof(ModuleConfiguration, name) + i, *tmp++);
        }

        success &= _storage.write32(offsetof(ModuleConfiguration, imageCRC), getModuleConfiguration()->imageCRC);

        success &= _storage.commit();

        return success;
    } // eraseUserConfiguration

    bool
    lock()
    {
        return _storage.lock();
    }

    bool
    unlock()
    {
        return _storage.unlock();
    }

private:
    Storage& _storage;
    bool     _ready;
};
}
}

#endif // ifndef HW_FLASH_HPP_
