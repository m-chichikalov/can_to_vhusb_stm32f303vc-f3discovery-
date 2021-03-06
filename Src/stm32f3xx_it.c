/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx.h"
#include "stm32f3xx_it.h"

/* External variables --------------------------------------------------------*/
//extern PCD_HandleTypeDef hpcd_USB_FS;
//extern CAN_HandleTypeDef hcan;
extern 	void USART1_IRQ_Handler(void);
extern  void CAN_IRQ_RX0_Handler(void);
extern  void CAN_IRQ_RX1_Handler(void);
extern  void CAN_IRQ_SCE_Handler(void);


/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

void NMI_Handler(void)
{
	while(1)
	{
	}
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{

}

/******************************************************************************/
/* STM32F3xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f3xx.s).                    */
/******************************************************************************/

void USART1_IRQHandler(void)
{
	USART1_IRQ_Handler();
}

void DMA1_Channel4_IRQHandler(void)
{
		__NOP();
}

void DMA1_Channel5_IRQHandler(void)
{
		__NOP();
}

void USB_LP_IRQHandler(void)
{
//  HAL_PCD_IRQHandler(&hpcd_USB_FS);
}

void CAN_SCE_IRQHandler(void)
{
	CAN_IRQ_SCE_Handler();
}
void CAN_RX1_IRQHandler(void)
{
	CAN_IRQ_RX1_Handler();
}
void USB_LP_CAN_RX0_IRQHandler(void)
{
	CAN_IRQ_RX0_Handler();
}
//void USB_HP_CAN_TX_IRQHandler(void)
//{
//	CAN_IRQ_RX0_Handler();
//}
