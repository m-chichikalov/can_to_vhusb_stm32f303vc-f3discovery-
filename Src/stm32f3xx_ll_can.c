/*
 * stm32f3xx_ll_can.c
 *
 *  Created on: 26 Sept. 2017
 *  Author: m.chichikaliv@outlook.com
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx.h"
#include "stm32f3xx_ll_can.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions ---------------------------------------------------------*/

ErrorStatus Can_Init_Mode(void){
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
	return SUCCESS;
}

ErrorStatus Can_Normal_Mode(void){
	uint32_t count = 0;
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

ErrorStatus Can_Init(CanInitStruct* Init) {
	if (Can_Init_Mode() != SUCCESS)
		return ERROR;

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
	if (Can_Normal_Mode() == SUCCESS)
		return SUCCESS;
	return ERROR;
}


ErrorStatus Can_DeInit(void) {
	uint32_t count = 0;

	/* Request sleep mode */
	SET_BIT  (CAN->MCR, CAN_MCR_SLEEP);
	CLEAR_BIT(CAN->MCR, CAN_MCR_INRQ);

	while (IS_BIT_CLEAR(CAN->MSR, CAN_MSR_SLAK)) {
		if (count >= CAN_WAIT_TIMEOUT) {
			return ERROR;
		}
		count++;
	}

	return SUCCESS;
}

/*
 *  	Identifier.
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


void CAN_TX(TXmessageCANstruct* txMessageCAN)
{
	uint32_t freeMailbox = (uint32_t)__CAN_GET_FREE_TX_MAILBOX();

	if (freeMailbox != TX_MAILBOXES_FULL)
	{
		CAN->sTxMailBox[freeMailbox].TDLR = ((uint32_t)(txMessageCAN->Data[0]<< 0) |
											 (uint32_t)(txMessageCAN->Data[1]<< 8) |
			                             	 (uint32_t)(txMessageCAN->Data[2]<<16) |
											 (uint32_t)(txMessageCAN->Data[3]<<24));

		CAN->sTxMailBox[freeMailbox].TDHR = ((uint32_t)(txMessageCAN->Data[4]<< 0) |
											 (uint32_t)(txMessageCAN->Data[5]<< 8) |
											 (uint32_t)(txMessageCAN->Data[6]<<16) |
											 (uint32_t)(txMessageCAN->Data[7]<<24));

		CAN->sTxMailBox[freeMailbox].TDTR = (0xffffffff & (uint32_t)txMessageCAN->DLC);



		// set STID or EXID & IDE & RTR
		if (txMessageCAN->IDE == CAN_ID_STD){
		CAN->sTxMailBox[freeMailbox].TIR = (0xffffffff & (uint32_t)(txMessageCAN->StdId << 21));
		} else {
		CAN->sTxMailBox[freeMailbox].TIR  = (0xffffffff & (uint32_t)(txMessageCAN->ExtId << 3 )); }
		CAN->sTxMailBox[freeMailbox].TIR |= (uint32_t)(txMessageCAN->IDE | txMessageCAN->RTR);
		// set TX request bit
		SET_BIT(CAN->sTxMailBox[freeMailbox].TIR, (1 << 0));
	} else
	{
//		return ; // Don't have empty mail boxes
		__NOP();
	}

//	return ; // OK
}


__weak void CAN_IRQ_RX0_Handler(void){
//
}

void CAN_IRQ_RX1_Handler(void){
	__NOP();
}

__weak void CAN_IRQ_SCE_Handler(void){
	__NOP();
}

/*******************************END OF FILE***********************************/
