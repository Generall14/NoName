#include "error.h"
#include "utils.h"

#define BITS_MASK GPIO_ODR_ODR8 | GPIO_ODR_ODR9 | GPIO_ODR_ODR10 | GPIO_ODR_ODR11 | GPIO_ODR_ODR12 | GPIO_ODR_ODR13 | GPIO_ODR_ODR14 | GPIO_ODR_ODR15
#define WAIT_BASE 0x7FFFF

uint8_t error_code = ERROR_UNDEFINED;

void raise_error(uint8_t code)
{
	error_code = code;
	error_handler();
}

void prim_delay(uint32_t time)
{
	volatile uint32_t t = time;
	while(t)
		t--;
}

void error_handler()
{
	__disable_irq();
	while(1)
	{
		CLEAR_BIT(GPIOB->ODR, BITS_MASK);
		for(int i=0;i<8;i++)
		{
			prim_delay(WAIT_BASE);
			SWAP_BIT(GPIOB->ODR, BITS_MASK);
		}
		prim_delay(WAIT_BASE);
		CLEAR_BIT(GPIOB->ODR, BITS_MASK);
		SET_BIT(GPIOB->ODR, error_code<<8);
		prim_delay(WAIT_BASE*24);
	}
}
