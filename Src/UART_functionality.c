/* --------------- Includes ----------- */
#include "stm32f3xx_ll_usart.h"

void UART_Send(uint32_t *buf, uint8_t len) {
	uint8_t i = 0;
	while (buf[i] != 0x00) {
		USART1->TDR = (uint16_t) (buf[i++]);
		while (!LL_USART_IsActiveFlag_TXE(USART1)) {
			__NOP();
		}
	}

}

void __USART1_Handler(void) {
	__NOP();

}
