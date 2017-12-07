/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
// create microrl object and pointer on it
microrl_t rl;
microrl_t * prl = &rl;

circular_buffer_t c_b;
circular_buffer_t* pc_b = &c_b;

char * bufDMAtoUSART[30];

//char bufDMAtoUSART[LENGTH_BUFFER];

xTaskHandle xTask1Handle;
xTaskHandle xTaskSendCanFrameToUsartHandle;
xTaskHandle xTaskCLIHandle;

/* Private variables ---------------------------------------------------------*/
const char *pcTextForTask1 = "Task 1 is running\n";

/* Private function prototypes -----------------------------------------------*/
extern void Init(void);
extern void SystemClock_Config(void);
extern void GPIO_Init(void);

void vExecute (void *pvParameters);
extern void execute (int argc, const char * const * argv);

/* Private function prototypes -----------------------------------------------*/
void vTaskToggleLed(void *pvParameters);
static void vTaskSendCanFrameToUsart(void *pvParameters);
//static void xTaskCommandReceived(void *pvParameters);
static void TX_CAN_Frame(TimerHandle_t xTimer);

int main(void) {
	/* MCU Configuration----------------------------------------------------------*/
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	/* System interrupt initialization*/

	Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	GPIO_Init();
	/* Initialize circle buffer  */
	c_b_init(pc_b);

#if ( configUSE_TRACE_FACILITY == 1 )
	vTraceEnable(TRC_START);
	vTraceEnable(TRC_INIT);
#endif

	BaseType_t xReturn = 0;
	xReturn = xTaskCreate(vTaskToggleLed,
			    "ToggelPinLed", 256,
				NULL, 1, &xTask1Handle);
	xReturn += xTaskCreate(vTaskSendCanFrameToUsart,
			    "Task_2", 256,
				NULL, 1, &xTaskSendCanFrameToUsartHandle);

	xReturn += xTaskCreate(vExecute,
			    "CLI_", 256,
				NULL, 1, &xTaskCLIHandle);


	// create timer which will send one frame every 500 ms
    // just for testing
	TimerHandle_t txTimerHandler = xTimerCreate("TXcan", 1000, pdTRUE, 0, TX_CAN_Frame);
	xReturn += xTimerStart(txTimerHandler, 0);

	/* Initialize CLI interface */
	microrl_init(prl, print);
	// Set callback for execute
	microrl_set_execute_callback (prl, xTaskCLIHandle);

//	Check that all tasks are created
	if (xReturn != (BaseType_t)
				4
//	if creation is failed we will stuck here
							  ){ for(;;){} }

	vTaskStartScheduler();

	/* Infinite loop */
	while (1) {}
}


/**
 * This function is executed in case of error occurrence.
 */
void _Error_Handler(char * file, int line) {
	/* User can add his own implementation to report the error return state */
	while (1) {
	}
}

void vTaskToggleLed(void *pvParameters) {
	for (;;) {
		LL_GPIO_TogglePin(GPIOE, PinLed);
        vTaskDelay(250);
	}
}

void vExecute (void *pvParameters) {
	for(;;){
		uint32_t pendingCommand = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (pendingCommand != 0) {
			int argc = microrl_get_argc(prl);
			const char * const * argv = microrl_get_argv(prl);
			execute(argc, argv);
		}
	}
}

static void vTaskSendCanFrameToUsart(void *pvParameters) {
	uint32_t pendingFrameFromCAN = 0;
	RXmessageCANstruct rxMessageCAN;
	for (;;) {
		pendingFrameFromCAN = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (pendingFrameFromCAN != 0) {
//			Receive from CAN pending frame.
			uint8_t pendingMessages = __CAN_GET_MSG_PENDING(CAN_BANK_FIFO0);
			if (pendingMessages != 0) {
//		read date from FIFO_0 output
				rxMessageCAN.IDE = (uint32_t)(CAN->sFIFOMailBox[CAN_BANK_FIFO0].RIR & (1 << 2));
				rxMessageCAN.RTR = (uint32_t)(CAN->sFIFOMailBox[CAN_BANK_FIFO0].RIR & (1 << 1));
				if (rxMessageCAN.IDE == CAN_ID_STD) {
					rxMessageCAN.StdId = (uint32_t)(CAN->sFIFOMailBox[CAN_BANK_FIFO0].RIR >> 21);
				} else {
					rxMessageCAN.ExtId = (uint32_t)(CAN->sFIFOMailBox[CAN_BANK_FIFO0].RIR >> 3);
				}
				rxMessageCAN.DLC = (uint32_t)(CAN->sFIFOMailBox[CAN_BANK_FIFO0].RDTR & (uint32_t)0x0f);
				rxMessageCAN.Data[0] = (uint8_t) (CAN->sFIFOMailBox[CAN_BANK_FIFO0].RDLR & 0x000000ff);
				rxMessageCAN.Data[1] = (uint8_t)((CAN->sFIFOMailBox[CAN_BANK_FIFO0].RDLR & 0x0000ff00) >> 8);
				rxMessageCAN.Data[2] = (uint8_t)((CAN->sFIFOMailBox[CAN_BANK_FIFO0].RDLR & 0x00ff0000) >> 16);
				rxMessageCAN.Data[3] = (uint8_t)((CAN->sFIFOMailBox[CAN_BANK_FIFO0].RDLR & 0xff000000) >> 24);

				rxMessageCAN.Data[4] = (uint8_t) (CAN->sFIFOMailBox[CAN_BANK_FIFO0].RDHR & 0x000000ff);
				rxMessageCAN.Data[5] = (uint8_t)((CAN->sFIFOMailBox[CAN_BANK_FIFO0].RDHR & 0x0000ff00) >> 8);
				rxMessageCAN.Data[6] = (uint8_t)((CAN->sFIFOMailBox[CAN_BANK_FIFO0].RDHR & 0x00ff0000) >> 16);
				rxMessageCAN.Data[7] = (uint8_t)((CAN->sFIFOMailBox[CAN_BANK_FIFO0].RDHR & 0xff000000) >> 24);
				__CAN_FIFO_RELEASE(CAN_BANK_FIFO0);
				__CAN_ENABLE_IT(CAN_IT_FMP0);
//			Format the string using sprintf
//			sprintf(bufDMAtoUSART, "StdID->0x%lX \r\n", rxMessageCAN.StdId);
//			Send this string to Terminal
				if (prl->cmdlen == 0){
					print("......help.......\n\r");
				}
			}
		}
	}
}



void CAN_IRQ_RX0_Handler(void){
//	Proceed reception of new message
	BaseType_t xHigherPriorityTaskWoken;
	uint8_t pendingMessages = __CAN_GET_MSG_PENDING(CAN_BANK_FIFO0);
	if (pendingMessages != 0U ) {
//		throw TaskNotify to vTaskSendCanFrameToUsart
		__CAN_DISABLE_IT(CAN_IT_FMP0);
		xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(xTaskSendCanFrameToUsartHandle, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

void USART1_IRQ_Handler(void) {
		if (LL_USART_IsActiveFlag_IDLE(USART1) & LL_USART_IsEnabledIT_IDLE(USART1)){
			// if I decide to put the whole command I should use IDLE interrupt routine here
		}
		else if (LL_USART_IsActiveFlag_RXNE(USART1)  & LL_USART_IsEnabledIT_RXNE(USART1)){
			microrl_insert_char (prl, LL_USART_ReceiveData8(USART1));
		}
		else if (LL_USART_IsActiveFlag_TXE(USART1) & LL_USART_IsEnabledIT_TXE(USART1)){
			if (c_b_get_free_space(pc_b) != (LEN_BUFFER-1))
				LL_USART_TransmitData8(USART1, c_b_get(pc_b));
			else
				LL_USART_DisableIT_TXE(USART1);
		}
}

// Temporary SoftTimer to throw messages...
static void TX_CAN_Frame(TimerHandle_t xTimer)
{
	TXmessageCANstruct txMessageCAN;
	txMessageCAN.StdId = 0x110;
	txMessageCAN.IDE = CAN_ID_STD;
	txMessageCAN.RTR = CAN_RTR_DATA;
	txMessageCAN.DLC = 8;
	txMessageCAN.Data[0] = 0x00;
	txMessageCAN.Data[1] = 0x01;
	txMessageCAN.Data[2] = 0x02;
	txMessageCAN.Data[3] = 0x03;
	txMessageCAN.Data[4] = 0x04;
	txMessageCAN.Data[5] = 0x05;
	txMessageCAN.Data[6] = 0x06;
	txMessageCAN.Data[7] = 0x07;
	CAN_TX(&txMessageCAN);
}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}

#endif
