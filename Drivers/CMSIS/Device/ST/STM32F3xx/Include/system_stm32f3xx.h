#ifndef __SYSTEM_STM32F3XX_H
#define __SYSTEM_STM32F3XX_H

#ifdef __cplusplus
 extern "C" {
#endif 
  /* This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      3) by calling HAL API function HAL_RCC_GetHCLKFreq()
      3) by calling HAL API function HAL_RCC_ClockConfig()
         Note: If you use this function to configure the system clock; then there
               is no need to call the 2 first functions listed above, since SystemCoreClock
               variable is updated automatically.
  */
extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */
extern const uint8_t AHBPrescTable[16];   /*!< AHB prescalers table values */
extern const uint8_t APBPrescTable[8];    /*!< APB prescalers table values */

extern void Init(void);
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);
extern void SystemClock_Config(void);
extern void GPIO_Init(void);



#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_STM32F3XX_H */
