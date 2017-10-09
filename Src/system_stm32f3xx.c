/**
 ******************************************************************************
 *
 * CMSIS Cortex-M4 Device Peripheral Access Layer System Source File.
 *
 * 1. This file provides two functions and one global variable to be called from
 *    user application:
 *      - SystemInit(): This function is called at startup just after reset and
 *                      before branch to main program. This call is made inside
 *                      the "startup_stm32f3xx.s" file.
 *
 *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
 *                                  by the user application to setup the SysTick
 *                                  timer or configure other parameters.
 *
 *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
 *                                 be called whenever the core clock is changed
 *                                 during program execution.
 *
 * 2. After each device reset the HSI (8 MHz) is used as system clock source.
 *    Then SystemInit() function is called, in "startup_stm32f3xx.s" file, to
 *    configure the system clock before to branch to main program.
 *
 * 3. This file configures the system clock as follows:
 *=============================================================================
 *                         Supported STM32F3xx device
 *-----------------------------------------------------------------------------
 *        System Clock source                    | HSI
 *-----------------------------------------------------------------------------
 *        SYSCLK(Hz)                             | 8000000
 *-----------------------------------------------------------------------------
 *        HCLK(Hz)                               | 8000000
 *-----------------------------------------------------------------------------
 *        AHB Prescaler                          | 1
 *-----------------------------------------------------------------------------
 *        APB2 Prescaler                         | 1
 *-----------------------------------------------------------------------------
 *        APB1 Prescaler                         | 1
 *-----------------------------------------------------------------------------
 *        USB Clock                              | DISABLE
 *-----------------------------------------------------------------------------
 *=============================================================================
 */
#include "main.h"

#if !defined  (HSE_VALUE) 
#define HSE_VALUE    ((uint32_t)8000000) /*!< Default value of the External oscillator in Hz.
                                                This value can be provided and adapted by the user application. */
#endif /* HSE_VALUE */

#if !defined  (HSI_VALUE)
#define HSI_VALUE    ((uint32_t)8000000) /*!< Default value of the Internal oscillator in Hz.
                                                This value can be provided and adapted by the user application. */
#endif /* HSI_VALUE */

/*!< Uncomment the following line if you need to relocate your vector Table in
 Internal SRAM. */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x0 /*!< Vector Table base offset field. */

/* This variable is updated in three ways:
 1) by calling CMSIS function SystemCoreClockUpdate()
 2) by calling HAL API function HAL_RCC_GetHCLKFreq()
 3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
 Note: If you use this function to configure the system clock there is no need to
 call the 2 first functions listed above, since SystemCoreClock variable is
 updated automatically.
 */

uint32_t SystemCoreClock = 8000000;

const uint8_t AHBPrescTable[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8,
		9 };
const uint8_t APBPrescTable[8] = { 0, 0, 0, 0, 1, 2, 3, 4 };

/**
 * @brief  Setup the microcontroller system
 *         Initialize the FPU setting, vector table location and the PLL configuration is reset.
 * @param  None
 * @retval None
 */
void SystemInit(void) {
	/* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2)); /* set CP10 and CP11 Full Access */
#endif

	/* Reset the RCC clock configuration to the default reset state ------------*/
	/* Set HSION bit */
	RCC->CR |= 0x00000001U;

	/* Reset CFGR register */
	RCC->CFGR &= 0xF87FC00CU;

	/* Reset HSEON, CSSON and PLLON bits */
	RCC->CR &= 0xFEF6FFFFU;

	/* Reset HSEBYP bit */
	RCC->CR &= 0xFFFBFFFFU;

	/* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE bits */
	RCC->CFGR &= 0xFF80FFFFU;

	/* Reset PREDIV1[3:0] bits */
	RCC->CFGR2 &= 0xFFFFFFF0U;

	/* Reset USARTSW[1:0], I2CSW and TIMs bits */
	RCC->CFGR3 &= 0xFF00FCCCU;

	/* Disable all interrupts */
	RCC->CIR = 0x00000000U;

#ifdef VECT_TAB_SRAM
	SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
#else
	SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
#endif
}

/**
 * @brief  Update SystemCoreClock variable according to Clock Register Values.
 *         The SystemCoreClock variable contains the core clock (HCLK), it can
 *         be used by the user application to setup the SysTick timer or configure
 *         other parameters.
 *
 * @note   Each time the core clock (HCLK) changes, this function must be called
 *         to update SystemCoreClock variable value. Otherwise, any configuration
 *         based on this variable will be incorrect.
 *
 * @note   - The system frequency computed by this function is not the real
 *           frequency in the chip. It is calculated based on the predefined
 *           constant and the selected clock source:
 *
 *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
 *
 *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
 *
 *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**)
 *             or HSI_VALUE(*) multiplied/divided by the PLL factors.
 *
 *         (*) HSI_VALUE is a constant defined in stm32f3xx_hal.h file (default value
 *             8 MHz) but the real value may vary depending on the variations
 *             in voltage and temperature.
 *
 *         (**) HSE_VALUE is a constant defined in stm32f3xx_hal.h file (default value
 *              8 MHz), user has to ensure that HSE_VALUE is same as the real
 *              frequency of the crystal used. Otherwise, this function may
 *              have wrong result.
 *
 *         - The result of this function could be not correct when using fractional
 *           value for HSE crystal.
 *
 * @param  None
 * @retval None
 */
void SystemCoreClockUpdate(void) {
	uint32_t tmp = 0, pllmull = 0, pllsource = 0, predivfactor = 0;

	/* Get SYSCLK source -------------------------------------------------------*/
	tmp = RCC->CFGR & RCC_CFGR_SWS;

	switch (tmp) {
	case RCC_CFGR_SWS_HSI: /* HSI used as system clock */
		SystemCoreClock = HSI_VALUE;
		break;
	case RCC_CFGR_SWS_HSE: /* HSE used as system clock */
		SystemCoreClock = HSE_VALUE;
		break;
	case RCC_CFGR_SWS_PLL: /* PLL used as system clock */
		/* Get PLL clock source and multiplication factor ----------------------*/
		pllmull = RCC->CFGR & RCC_CFGR_PLLMUL;
		pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;
		pllmull = (pllmull >> 18) + 2;

#if defined (STM32F302xE) || defined (STM32F303xE) || defined (STM32F398xx)
		predivfactor = (RCC->CFGR2 & RCC_CFGR2_PREDIV) + 1;
		if (pllsource == RCC_CFGR_PLLSRC_HSE_PREDIV)
		{
			/* HSE oscillator clock selected as PREDIV1 clock entry */
			SystemCoreClock = (HSE_VALUE / predivfactor) * pllmull;
		}
		else
		{
			/* HSI oscillator clock selected as PREDIV1 clock entry */
			SystemCoreClock = (HSI_VALUE / predivfactor) * pllmull;
		}
#else      
		if (pllsource == RCC_CFGR_PLLSRC_HSI_DIV2) {
			/* HSI oscillator clock divided by 2 selected as PLL clock entry */
			SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
		} else {
			predivfactor = (RCC->CFGR2 & RCC_CFGR2_PREDIV) + 1;
			/* HSE oscillator clock selected as PREDIV1 clock entry */
			SystemCoreClock = (HSE_VALUE / predivfactor) * pllmull;
		}
#endif /* STM32F302xE || STM32F303xE || STM32F398xx */
		break;
	default: /* HSI used as system clock */
		SystemCoreClock = HSI_VALUE;
		break;
	}
	/* Compute HCLK clock frequency ----------------*/
	/* Get HCLK prescaler */
	tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
	/* HCLK clock frequency */
	SystemCoreClock >>= tmp;
}

void Init(void) {

	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
//	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG); // turn on syscfg with ll

	NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4); // cmsis

	/* System interrupt init*/
	/* MemoryManagement_IRQn interrupt configuration */
	NVIC_SetPriority(MemoryManagement_IRQn,
			NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	/* BusFault_IRQn interrupt configuration */
	NVIC_SetPriority(BusFault_IRQn,
			NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	/* UsageFault_IRQn interrupt configuration */
	NVIC_SetPriority(UsageFault_IRQn,
			NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	/* SVCall_IRQn interrupt configuration */
	NVIC_SetPriority(SVCall_IRQn,
			NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	/* DebugMonitor_IRQn interrupt configuration */
	NVIC_SetPriority(DebugMonitor_IRQn,
			NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	/* PendSV_IRQn interrupt configuration */
	NVIC_SetPriority(PendSV_IRQn,
			NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	/* SysTick_IRQn interrupt configuration */
	NVIC_SetPriority(SysTick_IRQn,
			NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));

}

/** System Clock Configuration  */
void SystemClock_Config(void) {

	LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

	if (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1) {
		Error_Handler();
	}
	LL_RCC_HSE_EnableBypass();

	LL_RCC_HSE_Enable();

	/* Wait till HSE is ready */
	while (LL_RCC_HSE_IsReady() != 1) {

	}
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_6);

	LL_RCC_PLL_Enable();

	/* Wait till PLL is ready */
	while (LL_RCC_PLL_IsReady() != 1) {

	}
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);

	LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);

	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

	/* Wait till System clock is ready */
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {

	}
	LL_Init1msTick(48000000);

	LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);

	LL_SetSystemCoreClock(48000000);

	LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_SYSCLK);

	/* SysTick_IRQn interrupt configuration */
	NVIC_SetPriority(SysTick_IRQn,
			NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
}

/** Configure pins as
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */

void GPIO_Init(void) {

	/* GPIO Ports Clock Enable */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

	LL_GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Mode =                 LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.OutputType =           LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pin =                  PinLed;
	GPIO_InitStruct.Speed =                LL_GPIO_SPEED_FREQ_LOW;
	if ((LL_GPIO_Init(GPIOE, &GPIO_InitStruct)) != SUCCESS) {
		Error_Handler();
	}

	/*Init Uart */

	LL_USART_InitTypeDef USART_InitStruct;

	/* Peripheral clock enable */
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

	/**USART1 GPIO Configuration
	 PC4   ------> USART1_TX
	 PC5   ------> USART1_RX
	 */
	GPIO_InitStruct.Pin =                  LL_GPIO_PIN_4 | LL_GPIO_PIN_5;
	GPIO_InitStruct.Mode =                 LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed =                LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType =           LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull =                 LL_GPIO_PULL_UP;
	GPIO_InitStruct.Alternate =            LL_GPIO_AF_7;
	if ((LL_GPIO_Init(GPIOC, &GPIO_InitStruct)) != SUCCESS) {
		Error_Handler();
	}

	USART_InitStruct.BaudRate =            921600;
	USART_InitStruct.DataWidth =           LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits =            LL_USART_STOPBITS_1;
	USART_InitStruct.Parity =              LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection =   LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling =        LL_USART_OVERSAMPLING_16;
	if ((LL_USART_Init(USART1, &USART_InitStruct)) != SUCCESS) {
		Error_Handler();
	}

	LL_USART_DisableIT_CTS(USART1);
	LL_USART_DisableOverrunDetect(USART1);

	LL_USART_ConfigAsyncMode(USART1);

//	LL_USART_EnableDMAReq_RX(USART1);
//	LL_USART_EnableDMAReq_TX(USART1);
	LL_USART_ClearFlag_TC(USART1);

	LL_USART_Enable(USART1);

/* --------------------- DMA Init ----------------------------   */
//	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
//
//	LL_DMA_InitTypeDef DMA_InitStruct;
//
//	// config buffer->DMA->USART
//	DMA_InitStruct.Direction =              LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
//	DMA_InitStruct.NbData =                 LENGTH_BUFFER;
//	DMA_InitStruct.Mode =                   LL_DMA_MODE_NORMAL;
//	DMA_InitStruct.Priority =               LL_DMA_PRIORITY_LOW;
//	DMA_InitStruct.MemoryOrM2MDstIncMode =  LL_DMA_MEMORY_INCREMENT;
//	DMA_InitStruct.PeriphOrM2MSrcIncMode =  LL_DMA_PERIPH_NOINCREMENT;
//	DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
//	DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
//	DMA_InitStruct.MemoryOrM2MDstAddress =  (uint32_t)(&bufDMAtoUSART[0]);
//	DMA_InitStruct.PeriphOrM2MSrcAddress =  (uint32_t)(&(USART1->TDR));
//	if ((LL_DMA_Init(DMA1, LL_DMA_CHANNEL_4, &DMA_InitStruct)) != SUCCESS) {
//		Error_Handler();
//	}
	// config USART->DMA->buffer
//	DMA_InitStruct.Direction =              LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
//	DMA_InitStruct.NbData =                 LENGTH_BUFFER/2;
//	DMA_InitStruct.Mode =                   LL_DMA_MODE_NORMAL;
//	DMA_InitStruct.Priority =               LL_DMA_PRIORITY_LOW;
//	DMA_InitStruct.MemoryOrM2MDstIncMode =  LL_DMA_MEMORY_INCREMENT;
//	DMA_InitStruct.PeriphOrM2MSrcIncMode =  LL_DMA_PERIPH_NOINCREMENT;
//	DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
//	DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
//	DMA_InitStruct.MemoryOrM2MDstAddress =  (uint32_t)(&bufDMAtoUSART[15]);
//	DMA_InitStruct.PeriphOrM2MSrcAddress =  (uint32_t)(&(USART1->RDR));
//	if ((LL_DMA_Init(DMA1, LL_DMA_CHANNEL_5, &DMA_InitStruct)) != SUCCESS) {
//		Error_Handler();
//	}

//	LL_DMA_DisableIT_HT(DMA1, LL_DMA_CHANNEL_4);
//	LL_DMA_DisableIT_TC(DMA1, LL_DMA_CHANNEL_4);
//	LL_DMA_EnableIT_TE (DMA1, LL_DMA_CHANNEL_4);
//	LL_DMA_DisableIT_TE(DMA1, LL_DMA_CHANNEL_4);

//	LL_DMA_DisableIT_HT(DMA1, LL_DMA_CHANNEL_5);
//	LL_DMA_DisableIT_TC(DMA1, LL_DMA_CHANNEL_5);
//	LL_DMA_EnableIT_TE (DMA1, LL_DMA_CHANNEL_5);
////	LL_DMA_DisableIT_TE(DMA1, LL_DMA_CHANNEL_5);

//	NVIC_SetPriority(DMA1_Channel4_IRQn,0xf);
//	NVIC_EnableIRQ(DMA1_Channel4_IRQn);
//
//	NVIC_SetPriority(DMA1_Channel5_IRQn,0xf);
//	NVIC_EnableIRQ(DMA1_Channel5_IRQn);

//	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
//	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);

/*------------------------Init CAN pins---------------------------*/
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_CAN);
		/**CAN GPIO Configuration
		 PA11     ------> CAN_RX
		 PA12     ------> CAN_TX
		 */
		GPIO_InitStruct.Pin = LL_GPIO_PIN_11 | LL_GPIO_PIN_12;
		GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
		GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
		GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = LL_GPIO_AF_9;
		if ((LL_GPIO_Init(GPIOA, &GPIO_InitStruct)) != SUCCESS) {
			Error_Handler();
		}

		uint32_t BaudRadePrescaler;
		CanInitStruct InitStruct;

		BaudRadePrescaler = 16;
		InitStruct.BTR =   (
				/* Silent/Loop Back MODE                  */
							CAN_MODE_SILENT_LOOPBACK |
				/* Re-synchronization jump width          */
				                         CAN_SJW_1TQ |
				/* Time segment 1                         */
							             CAN_BS1_6TQ |
				/* Time segment 2                         */
		                                 CAN_BS2_8TQ |
			    /* Baud rate prescaler                    */
		         ((uint32_t) BaudRadePrescaler + 1U) );

		InitStruct.MCR = (
				/* Set the time triggered communication mode */
				          (0x0U << CAN_MCR_TTCM_Pos) |
				/* Set the automatic bus-off management      */
				          (0x0U << CAN_MCR_ABOM_Pos) |
				/* Set the automatic wake-up mode            */
						  (0x0U << CAN_MCR_AWUM_Pos) |
				/* Set the no automatic retransmission       */
						  (0x0U << CAN_MCR_NART_Pos) |
				/* Set the receive FIFO locked mode          */
						  (0x0U << CAN_MCR_RFLM_Pos) |
				/* Set the transmit FIFO priority            */
						  (0x0U << CAN_MCR_TXFP_Pos) |
				/* Debug freeze                              */
						  (0x1U << CAN_MCR_DBF_Pos)  );

//		Enable some CAN interrupts
		NVIC_SetPriority(USB_LP_CAN_RX0_IRQn, 0xb);
		NVIC_EnableIRQ(USB_LP_CAN_RX0_IRQn);
		NVIC_SetPriority(CAN_SCE_IRQn, 0xb);
		NVIC_EnableIRQ(CAN_SCE_IRQn);

		__CAN_ENABLE_IT(CAN_IT_FMP0);


		if ((Can_Init(&InitStruct)) != SUCCESS) {
			Error_Handler();
		}


		CanFilterInitStruct BankInit;
		BankInit.BankActivation = ENABLE;
		BankInit.FifoAssignment = CAN_BANK_FIFO0;
		BankInit.BankMode = CAN_BANKMODE_IDMASK;
		BankInit.BankScale = CAN_BANKSCALE_32BIT;
		BankInit.FiltersBankNumber = 0;
/*
 *    	Identifier.
 *    	Each bit of the register specifies the level of the corresponding bit of
 *    the expected identifier.
 *    0 - Dominant bit is expected.
 *    1 - Recessive bit is expected.
 *
 *    	Mask.
 *    	Each bit of the register specifies whether the bit of the associated identifier
 *    register must match with the corresponding identifier register of the filter.
 *    0 - Do not care, the bit is not used for the comparison.
 *    1 - Must match, the bit of the incoming identifier must have the same level has
 *    specified in the corresponding identifier register of the filter.
 *
 */
		BankInit.FxR1 = (0x000 << 21) | (0x00000 << 3) | (0x0 << 2) | (0x0 << 1);  // ID
		BankInit.FxR2 = (0x000 << 21) | (0x00000 << 3) | (0x0 << 2) | (0x0 << 1); // Mask

		if ((Can_FilterInit(&BankInit)) != SUCCESS) {
			Error_Handler();
		}


		NVIC_SetPriority(USART1_IRQn, 0xA);
		NVIC_EnableIRQ(USART1_IRQn);

		while (LL_USART_IsActiveFlag_IDLE(USART1)) {
		LL_USART_ClearFlag_IDLE(USART1);
		}
//		LL_USART_EnableIT_IDLE(USART1);
		LL_USART_EnableIT_TXE(USART1);
		LL_USART_EnableIT_RXNE(USART1);

}
