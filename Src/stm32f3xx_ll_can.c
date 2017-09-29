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
	MODIFY_REG(CAN->MCR,(uint32_t)0x000100FC,(uint32_t )Init->MCR);

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


ErrorStatus Can_DeInit(void) {
	uint32_t count = 0;

	/* Request sleep mode */
	SET_BIT(CAN->MCR, CAN_MCR_SLEEP);

	while (IS_BIT_SET(CAN->MSR, CAN_MSR_SLAK)) {
		if (count >= CAN_WAIT_TIMEOUT) {
			return ERROR;
		}
		count++;
	}

	return SUCCESS;
}

/*
 *
 *
 * ---32Bit Identifier Mask filter
 * Init.FxR1 = (STID[31:21] << 21) | (EXID[20:3] << 3) | (IDE << 2) | (RTR << 1);  // ID
 * Init.FxR2 = (STID[31:21] << 21) | (EXID[20:3] << 3) | (IDE << 2) | (RTR << 1); // Mask
 *
 *   ---32Bit Identifier List filter
 * Init.FxR1 = (STID[31:21] << 21) | (EXID[20:3] << 3) | (IDE << 2) | (RTR << 1);  // ID
 * Init.FxR2 = (STID[31:21] << 21) | (EXID[20:3] << 3) | (IDE << 2) | (RTR << 1); // ID (second)
 *
 *   ---16Bit Identifier Mask filter
 * Init.FxR1 = (STID_ID[15:5] << 5)    |(RTR_ID << 4)    |(IDE_ID << 3)    |(EXID_ID[2:0] << 0)   |
 *         	   (STID_MASK[15:5] << 21) |(RTR_MASK << 20) |(IDE_MASK << 19) |(EXID_MASK[2:0] << 16);
 * Init.FxR2 = (STID_ID[15:5] << 5)    |(RTR_ID << 4)    |(IDE_ID << 3)    |(EXID_ID[2:0] << 0)   |
 *    		   (STID_MASK[15:5] << 21) |(RTR_MASK << 20) |(IDE_MASK << 19) |(EXID_MASK[2:0] << 16);
 *
 *   ---16Bit Identifier List filter
 * Init.FxR1 = (STID_ID[15:5] << 5)   |(RTR_ID << 4)   |(IDE_ID << 3)   |(EXID_ID[2:0] << 0)   |
 *  		   (STID_ID[15:5] << 21)  |(RTR_ID << 20)  |(IDE_ID << 19)  |(EXID_ID[2:0] << 16);
 * Init.FxR2 = (STID_ID[15:5] << 5)   |(RTR_ID << 4)   |(IDE_ID << 3)   |(EXID_ID[2:0] << 0)   |
 *  		   (STID_ID[15:5] << 21)  |(RTR_ID << 20)  |(IDE_ID << 19)  |(EXID_ID[2:0] << 16);
 */

ErrorStatus Can_FilterInit(CanFilterInitStruct* Init) {

	/* Set the initialization mode for filters */
	SET_BIT(CAN->FMR, CAN_FMR_FINIT);

	uint8_t bankPosition = Init->FiltersBankNumber;
	/* set filter mode (CAN_BANKMODE_IDMASK | CAN_BANKMODE_IDLIST) */
	MODIFY_BIT(CAN->FM1R, Init->BankMode, bankPosition);
	/* set filter scale configuration (CAN_BANKSCALE_16BIT | CAN_BANKSCALE_32BIT) */
	MODIFY_BIT(CAN->FS1R, Init->BankScale, bankPosition);
	/* set filter FIFO assignment (CAN_BANK_FIFO0 | CAN_BANK_FIFO1) */
	MODIFY_BIT(CAN->FFA1R, Init->FifoAssignment, bankPosition);

	WRITE_REG(CAN->sFilterRegister[bankPosition].FR1, Init->FxR1);
	WRITE_REG(CAN->sFilterRegister[bankPosition].FR2, Init->FxR2);

	/* set filter Active (ENABLE | DISABLE) */
	MODIFY_BIT(CAN->FA1R, Init->BankActivation, bankPosition);

	/* Set the active mode for filters */
	CLEAR_BIT(CAN->FMR, CAN_FMR_FINIT);

	return SUCCESS;
}

/*******************************END OF FILE***********************************/
