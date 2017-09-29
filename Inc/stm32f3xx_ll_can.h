/*
 * stm32f3xx_ll_can.h
 *
 *  Created on: 26 Sept. 2017
 *  Author: m.chichikalov@outlook.com
 */

#ifndef STM32F3XX_LL_CAN_H_
#define STM32F3XX_LL_CAN_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Private types -------------------------------------------------------------*/
typedef struct {
	uint32_t MCR; //Master control register
	uint32_t BTR; //Bit timing register
} CanInitStruct;

typedef struct {
	uint8_t FiltersBankNumber ; // - Specifies the filter which will be initialized.
	uint8_t  FifoAssignment; // Specifies the FIFO (0 or 1U) which will be assigned to the filter.
	uint8_t  BankMode; // Specifies the bank mode to be initialized.
	uint8_t  BankScale; //Specifies the bank scale.
	uint8_t BankActivation; // Enable or disable the bank.
	uint32_t FxR1;
//	uint16_t FxR1H;
	uint32_t FxR2;
//	uint16_t FxR2H;
}CanFilterInitStruct;

//typedef struct {
//
//}CanFilter32BitMaskStruct;

 /* Exported constants --------------------------------------------------------*/
#define CAN_ERROR_NONE              (0x00000000U)  /*!< No error             */
#define CAN_ERROR_EWG               (0x00000001U)  /*!< EWG error            */
#define CAN_ERROR_EPV               (0x00000002U)  /*!< EPV error            */
#define CAN_ERROR_BOF               (0x00000004U)  /*!< BOF error            */
#define CAN_ERROR_STF               (0x00000008U)  /*!< Stuff error          */
#define CAN_ERROR_FOR               (0x00000010U)  /*!< Form error           */
#define CAN_ERROR_ACK               (0x00000020U)  /*!< Acknowledgment error */
#define CAN_ERROR_BR                (0x00000040U)  /*!< Bit recessive        */
#define CAN_ERROR_BD                (0x00000080U)  /*!< LEC dominant         */
#define CAN_ERROR_CRC               (0x00000100U)  /*!< LEC transfer error   */
#define CAN_ERROR_FOV0              (0x00000200U)  /*!< FIFO0 overrun error  */
#define CAN_ERROR_FOV1              (0x00000400U)  /*!< FIFO1 overrun error  */
#define CAN_ERROR_TXFAIL            (0x00000800U)  /*!< Transmit failure     */

#define CAN_INITSTATUS_FAILED       (0x00000000U)  /*!< CAN initialization failed */
#define CAN_INITSTATUS_SUCCESS      (0x00000001U)  /*!< CAN initialization OK */

#define CAN_MODE_NORMAL             ((uint32_t)0x00000000U)                              /*!< Normal mode   */
#define CAN_MODE_LOOPBACK           ((uint32_t)CAN_BTR_LBKM)                   /*!< Loopback mode */
#define CAN_MODE_SILENT             ((uint32_t)CAN_BTR_SILM)                   /*!< Silent mode   */
#define CAN_MODE_SILENT_LOOPBACK    ((uint32_t)(CAN_BTR_LBKM | CAN_BTR_SILM))  /*!< Loopback combined with silent mode */

#define CAN_SJW_1TQ                 ((uint32_t)0x00000000U)    /*!< 1 time quantum */
#define CAN_SJW_2TQ                 ((uint32_t)CAN_BTR_SJW_0)  /*!< 2 time quantum */
#define CAN_SJW_3TQ                 ((uint32_t)CAN_BTR_SJW_1)  /*!< 3 time quantum */
#define CAN_SJW_4TQ                 ((uint32_t)CAN_BTR_SJW)    /*!< 4 time quantum */

#define CAN_BS1_1TQ                 ((uint32_t)0x00000000U)                                 /*!< 1 time quantum  */
#define CAN_BS1_2TQ                 ((uint32_t)CAN_BTR_TS1_0)                                    /*!< 2 time quantum  */
#define CAN_BS1_3TQ                 ((uint32_t)CAN_BTR_TS1_1)                                    /*!< 3 time quantum  */
#define CAN_BS1_4TQ                 ((uint32_t)(CAN_BTR_TS1_1 | CAN_BTR_TS1_0))                  /*!< 4 time quantum  */
#define CAN_BS1_5TQ                 ((uint32_t)CAN_BTR_TS1_2)                                    /*!< 5 time quantum  */
#define CAN_BS1_6TQ                 ((uint32_t)(CAN_BTR_TS1_2 | CAN_BTR_TS1_0))                  /*!< 6 time quantum  */
#define CAN_BS1_7TQ                 ((uint32_t)(CAN_BTR_TS1_2 | CAN_BTR_TS1_1))                  /*!< 7 time quantum  */
#define CAN_BS1_8TQ                 ((uint32_t)(CAN_BTR_TS1_2 | CAN_BTR_TS1_1 | CAN_BTR_TS1_0))  /*!< 8 time quantum  */
#define CAN_BS1_9TQ                 ((uint32_t)CAN_BTR_TS1_3)                                    /*!< 9 time quantum  */
#define CAN_BS1_10TQ                ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_0))                  /*!< 10 time quantum */
#define CAN_BS1_11TQ                ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_1))                  /*!< 11 time quantum */
#define CAN_BS1_12TQ                ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_1 | CAN_BTR_TS1_0))  /*!< 12 time quantum */
#define CAN_BS1_13TQ                ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_2))                  /*!< 13 time quantum */
#define CAN_BS1_14TQ                ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_2 | CAN_BTR_TS1_0))  /*!< 14 time quantum */
#define CAN_BS1_15TQ                ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_2 | CAN_BTR_TS1_1))  /*!< 15 time quantum */
#define CAN_BS1_16TQ                ((uint32_t)CAN_BTR_TS1) /*!< 16 time quantum */

#define CAN_BS2_1TQ                 (0x00000000U)                                /*!< 1 time quantum */
#define CAN_BS2_2TQ                 ((uint32_t)CAN_BTR_TS2_0)                    /*!< 2 time quantum */
#define CAN_BS2_3TQ                 ((uint32_t)CAN_BTR_TS2_1)                    /*!< 3 time quantum */
#define CAN_BS2_4TQ                 ((uint32_t)(CAN_BTR_TS2_1 | CAN_BTR_TS2_0))  /*!< 4 time quantum */
#define CAN_BS2_5TQ                 ((uint32_t)CAN_BTR_TS2_2)                    /*!< 5 time quantum */
#define CAN_BS2_6TQ                 ((uint32_t)(CAN_BTR_TS2_2 | CAN_BTR_TS2_0))  /*!< 6 time quantum */
#define CAN_BS2_7TQ                 ((uint32_t)(CAN_BTR_TS2_2 | CAN_BTR_TS2_1))  /*!< 7 time quantum */
#define CAN_BS2_8TQ                 ((uint32_t)CAN_BTR_TS2)                      /*!< 8 time quantum */

#define CAN_MCR_DBF_Pos      (16U)
#define CAN_MCR_DBF_Msk      (0x1U << CAN_MCR_DBF_Pos)
#define CAN_MCR_DBF          CAN_MCR_DBF_Msk


#define CAN_BANKMODE_IDMASK       ((uint8_t)0x00U)  /*!< Identifier mask mode */
#define CAN_BANKMODE_IDLIST       ((uint8_t)0x01U)  /*!< Identifier list mode */

#define CAN_BANKSCALE_16BIT       ((uint8_t)0x00U)  /*!< Two 16-bit filters */
#define CAN_BANKSCALE_32BIT       ((uint8_t)0x01U)  /*!< One 32-bit filter  */

#define CAN_BANK_FIFO0            ((uint8_t)0x00U)  /*!< Filter FIFO 0 assignment for filter x */
#define CAN_BANK_FIFO1            ((uint8_t)0x01U)  /*!< Filter FIFO 1 assignment for filter x */

#define CAN_ID_STD                  (0x00000000U)  /*!< Standard Id */
#define CAN_ID_EXT                  (0x00000004U)  /*!< Extended Id */

#define CAN_RTR_DATA                (0x00000000U)  /*!< Data frame */
#define CAN_RTR_REMOTE              (0x00000002U)  /*!< Remote frame */

/* Transmit Flags */
#define CAN_FLAG_RQCP0             (0x00000500U)  /*!< Request MailBox0 flag         */
#define CAN_FLAG_RQCP1             (0x00000508U)  /*!< Request MailBox1 flag         */
#define CAN_FLAG_RQCP2             (0x00000510U)  /*!< Request MailBox2 flag         */
#define CAN_FLAG_TXOK0             (0x00000501U)  /*!< Transmission OK MailBox0 flag */
#define CAN_FLAG_TXOK1             (0x00000509U)  /*!< Transmission OK MailBox1 flag */
#define CAN_FLAG_TXOK2             (0x00000511U)  /*!< Transmission OK MailBox2 flag */
#define CAN_FLAG_TME0              (0x0000051AU)  /*!< Transmit mailbox 0 empty flag */
#define CAN_FLAG_TME1              (0x0000051BU)  /*!< Transmit mailbox 0 empty flag */
#define CAN_FLAG_TME2              (0x0000051CU)  /*!< Transmit mailbox 0 empty flag */

/* Receive Flags */
#define CAN_FLAG_FF0               (0x00000203U)  /*!< FIFO 0 Full flag    */
#define CAN_FLAG_FOV0              (0x00000204U)  /*!< FIFO 0 Overrun flag */

#define CAN_FLAG_FF1               (0x00000403U)  /*!< FIFO 1 Full flag    */
#define CAN_FLAG_FOV1              (0x00000404U)  /*!< FIFO 1 Overrun flag */

/* Operating Mode Flags */
#define CAN_FLAG_INAK              (0x00000100U)  /*!< Initialization acknowledge flag */
#define CAN_FLAG_SLAK              (0x00000101U)  /*!< Sleep acknowledge flag          */
#define CAN_FLAG_ERRI              (0x00000102U)  /*!< Error flag                      */
#define CAN_FLAG_WKU               (0x00000103U)  /*!< Wake up flag                    */
#define CAN_FLAG_SLAKI             (0x00000104U)  /*!< Sleep acknowledge flag          */
/* @note When SLAK interrupt is disabled (SLKIE=0U), no polling on SLAKI is possible.
	  In this case the SLAK bit can be polled.*/

/* Error Flags */
#define CAN_FLAG_EWG               (0x00000300U)  /*!< Error warning flag   */
#define CAN_FLAG_EPV               (0x00000301U)  /*!< Error passive flag   */
#define CAN_FLAG_BOF               (0x00000302U)  /*!< Bus-Off flag         */

/* ---  CAN_interrupts CAN Interrupts ----*/
#define CAN_IT_TME                  ((uint32_t)CAN_IER_TMEIE)   /*!< Transmit mailbox empty interrupt */

/* Receive Interrupts */
#define CAN_IT_FMP0                 ((uint32_t)CAN_IER_FMPIE0)  /*!< FIFO 0 message pending interrupt */
#define CAN_IT_FF0                  ((uint32_t)CAN_IER_FFIE0)   /*!< FIFO 0 full interrupt            */
#define CAN_IT_FOV0                 ((uint32_t)CAN_IER_FOVIE0)  /*!< FIFO 0 overrun interrupt         */
#define CAN_IT_FMP1                 ((uint32_t)CAN_IER_FMPIE1)  /*!< FIFO 1 message pending interrupt */
#define CAN_IT_FF1                  ((uint32_t)CAN_IER_FFIE1)   /*!< FIFO 1 full interrupt            */
#define CAN_IT_FOV1                 ((uint32_t)CAN_IER_FOVIE1)  /*!< FIFO 1 overrun interrupt         */

/* Operating Mode Interrupts */
#define CAN_IT_WKU                  ((uint32_t)CAN_IER_WKUIE)  /*!< Wake-up interrupt           */
#define CAN_IT_SLK                  ((uint32_t)CAN_IER_SLKIE)  /*!< Sleep acknowledge interrupt */

/* Error Interrupts */
#define CAN_IT_EWG                  ((uint32_t)CAN_IER_EWGIE) /*!< Error warning interrupt   */
#define CAN_IT_EPV                  ((uint32_t)CAN_IER_EPVIE) /*!< Error passive interrupt   */
#define CAN_IT_BOF                  ((uint32_t)CAN_IER_BOFIE) /*!< Bus-off interrupt         */
#define CAN_IT_LEC                  ((uint32_t)CAN_IER_LECIE) /*!< Last error code interrupt */
#define CAN_IT_ERR                  ((uint32_t)CAN_IER_ERRIE) /*!< Error Interrupt           */

/* Mailboxes definition */
#define CAN_TXMAILBOX_0   ((uint8_t)0x00U)
#define CAN_TXMAILBOX_1   ((uint8_t)0x01U)
#define CAN_TXMAILBOX_2   ((uint8_t)0x02U)


/* Exported macros -----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
 ErrorStatus Can_Init(CanInitStruct* Init);
 ErrorStatus Can_DeInit(void);
 ErrorStatus Can_FilterInit(CanFilterInitStruct* Init);

/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
#define CAN_TXSTATUS_NOMAILBOX      ((uint8_t)0x04U)  /*!< CAN cell did not provide CAN_TxStatus_NoMailBox */
#define CAN_FLAG_MASK               (0x000000FFU)
#define CAN_WAIT_TIMEOUT            ((uint32_t)0x00000200)

/* Private Macros -----------------------------------------------------------*/
#define IS_BIT_CLEAR(REG, BIT)   (! ((REG) & (BIT)))
#define IS_BIT_SET(REG, BIT)     ((REG) & (BIT))

#define MODIFY_BIT(REG, MEANING, POS)  (REG) = (((REG) & (~(1 << POS))) | (MEANING << POS))

#ifdef __cplusplus
 } /*  __cplusplus  */
#endif

#endif /* STM32F3XX_LL_CAN_H_ */
