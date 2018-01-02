/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */


#pragma once

#include <board.h>

#include "stm32f3xx.h"
#include <stdint.h>

#define  FLASH_OBR_RDPRT1                    ((uint32_t)0x00000002)        /*!< Read protection Level 1 */
#define  FLASH_OBR_RDPRT2                    ((uint32_t)0x00000004)        /*!< Read protection Level 2 */

#define assert_param(expr) ((void)0)
