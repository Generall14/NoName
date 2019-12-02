#include "main.h"
#include "usb_device.h"
#include "utils.h"
#include "error.h"
#include "core_cm3.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"
#include "SProt/sprot_i.h"
#include "SProt/sprot_l.h"
#include "global_clock.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

int main(void)
{
	volatile uint32_t last_timestamp, current_timestamp, diff_timestamp, flash_erase_time, flash_write_time, flash_unlock_time;
	volatile uint32_t flash_read_time, flash_read;

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	SystemClock_Config();

	sprot_init_fifo(&pc_fifo);
	sprot_init_fifo(&rpc_fifo);

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USB_DEVICE_Init();
	MX_TIM2_Init();

	//FLASH test
#ifdef xxxxxxxxxxxxxxxx
	uint32_t PageError;
	FLASH_EraseInitTypeDef ddd= {FLASH_TYPEERASE_PAGES, 0, 0x801F800u, 1};
	last_timestamp = get_global_clock();
	HAL_FLASH_Unlock();
	current_timestamp = get_global_clock();
	flash_unlock_time = current_timestamp - last_timestamp;

	last_timestamp = get_global_clock();
	HAL_FLASHEx_Erase(&ddd, &PageError);
	current_timestamp = get_global_clock();
	flash_erase_time = current_timestamp - last_timestamp;

	for(uint32_t i=0; i<16; i++)
	{
		last_timestamp = get_global_clock();
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x801F800u+4*i, 0x11111111*i);
		current_timestamp = get_global_clock();
		flash_write_time = current_timestamp - last_timestamp;
	}

	HAL_FLASH_Lock();
	HAL_FLASH_Unlock();
	for(uint32_t i=0; i<16; i++)
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x801F800u+4*i+4*16, 0x11111111*i);
	HAL_FLASH_Lock();

	last_timestamp = get_global_clock();
	uint32_t* ptr = 0x801F800u+4*2+4;
	flash_read = *ptr;
	current_timestamp = get_global_clock();
	flash_read_time = current_timestamp - last_timestamp;
#endif

	while (1)
	{
		blocking_delay_ms(1);
		process_fifo(&pc_fifo, &rpc_fifo, ififo_tbl, ififo_def, ififo_tbl_size);
		process_fifo(&rpc_fifo, 0, irfifo_tbl, irfifo_def, irfifo_tbl_size);

		LED_SWAP(LED0);
		LED_ON(LED1);
		LED_OFF(LED2);

		last_timestamp = current_timestamp;
		current_timestamp = get_global_clock();
		diff_timestamp = current_timestamp - last_timestamp;
	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

	if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
	{
		Error_Handler();
	}
	LL_RCC_HSE_Enable();

	/* Wait till HSE is ready */
	while(LL_RCC_HSE_IsReady() != 1)
	{

	}
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
	LL_RCC_PLL_Enable();

	/* Wait till PLL is ready */
	while(LL_RCC_PLL_IsReady() != 1)
	{

	}
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

	/* Wait till System clock is ready */
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
	{

	}
	LL_Init1msTick(72000000);
	LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
	LL_SetSystemCoreClock(72000000);
	LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLL_DIV_1_5);


	WRITE_REG(SysTick->LOAD, 0x1193f); // 1 ms
	LL_SYSTICK_EnableIT();
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

	/**/
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_10|LL_GPIO_PIN_11|LL_GPIO_PIN_12|LL_GPIO_PIN_13
	                       |LL_GPIO_PIN_14|LL_GPIO_PIN_15|LL_GPIO_PIN_8|LL_GPIO_PIN_9);

	/**/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/**/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_10|LL_GPIO_PIN_11|LL_GPIO_PIN_12|LL_GPIO_PIN_13
	                      |LL_GPIO_PIN_14|LL_GPIO_PIN_15|LL_GPIO_PIN_8|LL_GPIO_PIN_9;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

static void MX_TIM2_Init(void)
{
	// Tick 1 us
	// UIF every 0xFFFF * 1 us
	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	LL_TIM_InitTypeDef TIM_InitStruct = {0};

	/* Peripheral clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

	/* TIM2 interrupt Init */
	NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	NVIC_EnableIRQ(TIM2_IRQn);

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	TIM_InitStruct.Prescaler = 71;
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.Autoreload = 0xFFFF;
	TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	LL_TIM_Init(TIM2, &TIM_InitStruct);
	LL_TIM_DisableARRPreload(TIM2);
	LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
	LL_TIM_DisableMasterSlaveMode(TIM2);

	LL_TIM_EnableIT_UPDATE(TIM2);
	LL_TIM_EnableCounter(TIM2);
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
