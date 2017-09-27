/*
 * stm32f3xx_ll_can.c
 *
 *  Created on: 26 Sept. 2017
 *  Author: m.chichikaliv@outlook.com
 */
/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx.h"
#include "stm32f3xx_ll_can.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions ---------------------------------------------------------*/

ErrorStatus Can_Init(CanInitStruct* Init) {
	uint32_t count = 0;
	/* Exit from sleep mode */
	CLEAR_BIT(CAN->MCR, CAN_MCR_SLEEP);

	/* Request initialization */
	SET_BIT(CAN->MCR, CAN_MCR_INRQ);

	/* Wait the acknowledge */
	//todo Start using SysTick
	while (IS_BIT_CLEAR(CAN->MSR, CAN_MSR_INAK)) {
		if (count >= CAN_WAIT_TIMEOUT) {
			return ERROR;
		}
		count++;
	}

	/* Set the time triggered communication mode */

	/* Set the automatic bus-off management */

	/* Set the automatic wake-up mode */

	/* Set the no automatic retransmission */

	/* Set the receive FIFO locked mode */

	/* Set the transmit FIFO priority */

	/* Set the Master Control register */
	CAN->MCR |= (uint32_t )Init->MCR;

	/* Set the Bit Timing register */
	WRITE_REG(CAN->BTR, (uint32_t )(Init->BTR));

	/* Request leave initialization */
	CLEAR_BIT(CAN->MCR, CAN_MCR_INRQ);

	/* Wait the acknowledge */
	//todo Start using SysTick
	count = 0x00;
	while (IS_BIT_SET(CAN->MSR, CAN_MSR_INAK)) {
		if (count >= CAN_WAIT_TIMEOUT) {
			return ERROR;
		}
		count++;
	}
	return SUCCESS;
}

/*******************************END OF FILE***********************************/
