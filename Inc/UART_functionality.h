/*
 * UART_functionality.h
 *
 *  UART_Send(uint32_t *buf, uint8_t len);
 *
 *  Created on: 24 сент. 2017 г.
 *  Author: m.chichikalov@outlook.com
 */

#ifndef UART_FUNCTIONALITY_H_
#define UART_FUNCTIONALITY_H_

void __USART1_Handler(void);
void UART_Send(uint32_t *buf, uint8_t len);

#endif /* UART_FUNCTIONALITY_H_ */
