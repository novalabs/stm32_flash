// This is a very crappy workaround...
#if defined(STM32F303xx)
  #include "stm32f30x_flash.impl"
#elif defined(STM32F091xC)
  #define assert_param(expr) ((void)0)
  #include "stm32f0xx_flash.impl"
#else
  #error "Chip not supported"
#endif

