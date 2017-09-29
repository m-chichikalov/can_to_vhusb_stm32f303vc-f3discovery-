/* todo
 *
 * 1 -  receive packets from CAN
 * 1a - init CAN
 * 2 - send them to vhost usb
 */

//#define DEBUG_SEMIHOSTING

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/
//CAN_HandleTypeDef hcan;  // need HAL

/*--- buffer for DMA USART1-TX ----------*/
char bufDMAtoUSART[50];

xTaskHandle xTask1Handle;
xTaskHandle xTask2Handle;

/* Private variables ---------------------------------------------------------*/
const char *pcTextForTask1 = "Task 1 is running\n";
const char *pcTextForTask2 = "Task 2 is running\n";

/* Private function prototypes -----------------------------------------------*/
extern void Init(void);
extern void SystemClock_Config(void);
extern void GPIO_Init(void);

extern void initialise_monitor_handles(void); /* prototype */

/* Private function prototypes -----------------------------------------------*/
void vTaskSet(void *pvParameters);
void vTaskReset(void *pvParameters);

int main(void) {
	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	/* System interrupt init*/
	Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Turn on the semihosting */
	initialise_monitor_handles();
	printf("hello world!\r\n");

	/* Initialize all configured peripherals */
	GPIO_Init();


#if ( configUSE_TRACE_FACILITY == 1 )
	vTraceEnable(TRC_START);
	vTraceEnable(TRC_INIT);
#endif

	xTaskCreate(vTaskSet, "TSetPinLed", 256, (void*) pcTextForTask1, 1, &xTask1Handle);
//	  xTaskCreate(vTaskReset, "Task_2", 256, (void*)pcTextForTask2, 1, &xTask2Handle);
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

void vTaskSet(void *pvParameters) {
//	const char * buf = "Hello world!";
//	uint32_t i;
	for (;;) {
		LL_GPIO_TogglePin(GPIOE, PinLed);
		bufDMAtoUSART[0] += 1;
		bufDMAtoUSART[1] += 2;
		vTaskDelay(500);

	}
}

void vTaskReset(void *pvParameters) {
	for (;;) {
		LL_GPIO_ResetOutputPin(GPIOE, PinLed);
		vTaskDelay(500);

	}
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
