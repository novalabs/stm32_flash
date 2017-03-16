/*
 * stm32f30x.h
 *
 *  Created on: 5 May 2016
 *      Author: davide
 */

#ifndef STM32F30X_H_
#define STM32F30X_H_

#include <board.h>

#include "stm32f3xx.h"
#include <stdint.h>

#define  FLASH_OBR_RDPRT1                    ((uint32_t)0x00000002)        /*!< Read protection Level 1 */
#define  FLASH_OBR_RDPRT2                    ((uint32_t)0x00000004)        /*!< Read protection Level 2 */

#define assert_param(expr) ((void)0)

#endif /* STM32F30X_H_ */
