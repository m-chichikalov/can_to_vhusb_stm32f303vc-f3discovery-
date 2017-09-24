/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
extern void Init(void);
extern void SystemClock_Config(void);
extern void MX_GPIO_Init(void);

extern void initialise_monitor_handles(void); /* prototype */

/* Private function prototypes -----------------------------------------------*/

int main(void) {
	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	/* System interrupt init*/
	Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Turn on the semihosting */
	initialise_monitor_handles();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();

	/* Infinite loop */
	while (1) {
		LL_GPIO_SetOutputPin(GPIOE, PinLed);
		printf("hello world!\r\n");
	}
}

/**
 * This function is executed in case of error occurrence.
 */
void _Error_Handler(char * file, int line) {
	/* User can add his own implementation to report the error return state */
	while (1) {
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
