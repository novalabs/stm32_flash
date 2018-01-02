/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#ifndef FLASH_SEGMENTS_HPP_
#define FLASH_SEGMENTS_HPP_

#ifdef CORE_IS_BOOTLOADER
#if CORE_IS_BOOTLOADER
extern const uint32_t user_address_bottom[];
extern const uint32_t user_address_top[];
#endif
#endif

#if TAGS_SIZE > 0
extern const uint32_t tags_address_bottom[];
extern const uint32_t tags_address_top[];
#endif

extern const uint32_t conf1_address_bottom[];
extern const uint32_t conf1_address_top[];
extern const uint32_t conf2_address_bottom[];
extern const uint32_t conf2_address_top[];

namespace core {
namespace stm32_flash {
#include <stdint.h>

#ifdef CORE_IS_BOOTLOADER
#if CORE_IS_BOOTLOADER
static const uint32_t PROGRAM_FLASH_FROM = reinterpret_cast<uint32_t>(user_address_bottom);
static const uint32_t PROGRAM_FLASH_TO   = reinterpret_cast<uint32_t>(user_address_top);
static const uint32_t PROGRAM_FLASH_SIZE = PROGRAM_FLASH_TO - PROGRAM_FLASH_FROM;
static const uint32_t PROGRAM_JUMP       = reinterpret_cast<uint32_t>(user_address_bottom);
#endif
#endif

#if TAGS_SIZE > 0
static const uint32_t TAGS_FLASH_FROM = reinterpret_cast<uint32_t>(tags_address_bottom);
static const uint32_t TAGS_FLASH_TO   = reinterpret_cast<uint32_t>(tags_address_top);
static const uint32_t TAGS_FLASH_SIZE = TAGS_FLASH_TO - TAGS_FLASH_FROM;
#else
static const uint32_t TAGS_FLASH_SIZE = 0;
#endif

static const uint32_t CONFIGURATION1_FLASH_FROM = reinterpret_cast<uint32_t>(conf1_address_bottom);
static const uint32_t CONFIGURATION1_FLASH_TO   = reinterpret_cast<uint32_t>(conf1_address_top);
static const uint32_t CONFIGURATION1_FLASH_SIZE = CONFIGURATION1_FLASH_TO - CONFIGURATION1_FLASH_FROM;

static const uint32_t CONFIGURATION2_FLASH_FROM = reinterpret_cast<uint32_t>(conf2_address_bottom);
static const uint32_t CONFIGURATION2_FLASH_TO   = reinterpret_cast<uint32_t>(conf2_address_top);
static const uint32_t CONFIGURATION2_FLASH_SIZE = CONFIGURATION2_FLASH_TO - CONFIGURATION2_FLASH_FROM;
}
}


#endif // ifndef FLASH_SEGMENTS_HPP_
