//#define DEBUG_SEMIHOSTING

/* Includes ------------------------------------------------------------------*/
#include "main.h"
//#include <stdio.h>

/* Private variables ---------------------------------------------------------*/

/*--- buffer for DMA USART1-TX ----------*/
char bufDMAtoUSART[50];

xTaskHandle xTask1Handle, xTaskSendCanFrameToUsartHandle;

/* Private variables ---------------------------------------------------------*/
const char *pcTextForTask1 = "Task 1 is running\n";
const char *pcTextForTask2 = "Task 2 is running\n";

/* Private function prototypes -----------------------------------------------*/
extern void Init(void);
extern void SystemClock_Config(void);
extern void GPIO_Init(void);

extern void initialise_monitor_handles(void); /* prototype */

/* Private function prototypes -----------------------------------------------*/
void vTaskToggleLed(void *pvParameters);
static void vTaskSendCanFrameToUsart(void *pvParameters);
static void TX_CAN_Frame(TimerHandle_t xTimer);

int main(void) {
	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	/* System interrupt init*/
	Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Turn on the semihosting */
//	initialise_monitor_handles();
//	printf("hello world!\r\n");

	/* Initialize all configured peripherals */
	GPIO_Init();


#if ( configUSE_TRACE_FACILITY == 1 )
	vTraceEnable(TRC_START);
	vTraceEnable(TRC_INIT);
#endif

	xTaskCreate(vTaskToggleLed, "ToggelPinLed", 256, (void*) pcTextForTask1, 0, &xTask1Handle);
	xTaskCreate(vTaskSendCanFrameToUsart,
			    "Task_2", 512,
				(void*)pcTextForTask2, 1, &xTaskSendCanFrameToUsartHandle);

	// create timer which will send one frame every 500 ms
    // just for testing
	TimerHandle_t txTimer = xTimerCreate("TXcan", 1000, pdTRUE, 0, TX_CAN_Frame);
	xTimerStart(txTimer, 0);
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
		vTaskDelay(500);
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
				sprintf(bufDMAtoUSART, "StdID = 0x%lX \r\n", rxMessageCAN.StdId);
//			Send this string to DMA
				LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
				LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, LENGTH_BUFFER);
				LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
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
