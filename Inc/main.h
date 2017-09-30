#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/
//#include "stm32f3xx_ll_adc.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_can.h"
//#include "stm32f3xx_ll_comp.h"
#include "stm32f3xx_ll_cortex.h"
//#include "stm32f3xx_ll_crc.h"
//#include "stm32f3xx_ll_dac.h"
#include "stm32f3xx_ll_dma.h"
//#include "stm32f3xx_ll_exti.h"
#if defined  (USE_HAL_DRIVER)
	#include "stm32f3xx_ll_fmc.h"
#endif /* USE_HAL_DRIVER */
#include "stm32f3xx_ll_gpio.h"
//#include "stm32f3xx_ll_hrtim.h"
//#include "stm32f3xx_ll_i2c.h"
//#include "stm32f3xx_ll_iwdg.h"
//#include "stm32f3xx_ll_opamp.h"
#include "stm32f3xx_ll_pwr.h"
#include "stm32f3xx_ll_rcc.h"
#include "stm32f3xx_ll_rtc.h"
//#include "stm32f3xx_ll_spi.h"
#include "stm32f3xx_ll_system.h"
//#include "stm32f3xx_ll_tim.h"
#include "stm32f3xx_ll_usart.h"
#include "stm32f3xx_ll_utils.h"
//#include "stm32f3xx_ll_wwdg.h"
#include "stm32f3xx.h"

#include "UART_functionality.h"

#include "FreeRTOS.h"
#include "task.h"
#include "list.h"
#include "timers.h"

/* Private define ------------------------------------------------------------*/
#define PinLed GPIO_BSRR_BS_9  // led NORTH))

#define LENGTH_BUFFER 5

/* define DEBUG_SEMIHOSTIG to turn it on  */


#if !defined  (DEBUG_SEMIHOSTING)
	#define printf(__NN__) ((void)0U)
	#define initialise_monitor_handles __NOP
#else
	#include <stdio.h>
#endif /* USE_HAL_DRIVER */


#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif

/* ########################## Assert Selection ############################## */
/**
  * Uncomment the line below to expanse the "assert_param" macro in the
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1 */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

#endif /* __MAIN_H */
