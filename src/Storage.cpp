/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <osal.h>
#include <core/stm32_flash/Storage.hpp>
#include <core/stm32_crc/CRC.hpp>

#include <algorithm>

namespace core {
namespace stm32_flash {
Storage::Storage(
    FlashSegment& bank1,
    FlashSegment& bank2
) : _bank1(bank1), _bank2(bank2), _cnt(0xFFFF), _readBank(nullptr), _writeBank(nullptr), _head(0), _writeReady(false), _bankSize(0)
{
    uint16_t cnt1       = _bank1.read16_offset(CNT_OFFSET);
    uint16_t cnt2       = _bank2.read16_offset(CNT_OFFSET);
    uint32_t crc1       = _bank1.read32_offset(CRC_OFFSET);
    uint32_t crc2       = _bank2.read32_offset(CRC_OFFSET);
    bool     bank1Valid = false;
    bool     bank2Valid = false;

    if (cnt1 != 0xFFFF) {
        if (crc1 == getBankCRC(_bank1)) {
            bank1Valid = true;
        }
    }

    if (cnt2 != 0xFFFF) {
        if (crc2 == getBankCRC(_bank2)) {
            bank2Valid = true;
        }
    }

    if (bank1Valid && bank2Valid) {
        // Both banks are valid, choose the newest
        if (cnt1 > cnt2) {
            _cnt       = cnt1;
            _head      = _bank1.from() + DATA_OFFSET;
            _readBank  = &_bank1;
            _writeBank = &_bank2;
        } else if (cnt2 > cnt1) {
            _cnt       = cnt2;
            _head      = _bank2.from() + DATA_OFFSET;
            _readBank  = &_bank2;
            _writeBank = &_bank1;
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
        _readBank  = nullptr;
    }

    _bankSize = std::min(_bank1.size(), _bank2.size());
}

bool
Storage::erase()
{
    bool success = true;

    success &= _bank1.erase();
    success &= _bank2.erase();

    osalSysLock();

    _cnt       = 0xFFFF;
    _readBank  = nullptr;
    _writeBank = &_bank1;

    osalSysUnlock();

    return success;
}

bool
Storage::format()
{
    osalSysLock();

    if (_writeReady) {
        osalSysUnlock();
        return false;
    }

    // write bank is always defined
    _writeBank->unlock();
    _writeReady = _writeBank->erase();

    osalSysUnlock();

    return _writeReady;
}

bool
Storage::commit()
{
    osalSysLock();

    if (!_writeReady) {
        osalSysUnlock();
        return false;
    }

    _writeReady = false;

    _cnt++;

    if (_cnt == 0xFFFF) {
        _cnt = 0;
    }

    bool success = true;

    // write bank is always defined
    success &= _writeBank->write16_offset(CNT_OFFSET, _cnt);
    success &= _writeBank->write32_offset(CRC_OFFSET, getBankCRC(*_writeBank));

    _writeBank->lock();

    // Swap the 2 banks
    FlashSegment* tmp = _readBank;

    if (tmp == nullptr) {
        // It was invalid. We cannot assign it to write...
        if (_writeBank == &_bank1) {
            tmp = &_bank2;
        } else {
            tmp = &_bank1;
        }
    }

    _readBank  = _writeBank;
    _writeBank = tmp;

    osalSysUnlock();

    return success;
} // commit

bool
Storage::isValid() const
{
    osalSysLock();

    bool valid = _readBank != nullptr;

    osalSysUnlock();

    return valid;
}

bool
Storage::lock()
{
    return _writeBank->lock();
}

bool
Storage::unlock()
{
    return _writeBank->unlock();
}

uint32_t
Storage::getBankCRC(
    FlashSegment& bank
)
{
    if (((bank.size() - DATA_OFFSET) % 4) != 0) {
        chSysHalt("Data size not multiple of 4");
    }

    core::stm32_crc::CRC::init();
    core::stm32_crc::CRC::setPolynomialSize(core::stm32_crc::CRC::PolynomialSize::POLY_32);
    return core::stm32_crc::CRC::CRCBlock(reinterpret_cast<uint32_t*>(bank.from() + DATA_OFFSET), (bank.size() - DATA_OFFSET) / sizeof(uint32_t));
}
}
}
