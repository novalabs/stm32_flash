/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#if defined(STM32F303xx)
    #define assert_param(expr) ((void)0)
    #include "stm32f30x_flash.impl"
#elif defined(STM32F091xC)
    #define assert_param(expr) ((void)0)
    #include "stm32f0xx_flash.impl"
#elif defined(STM32F407xx)
    #define assert_param(expr) ((void)0)
    #include "stm32f4xx_flash.impl"
#else
    #error "Chip not supported"
#endif

