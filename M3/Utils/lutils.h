#ifndef _LUTILS_H_
#define _LUTILS_H_

#define LED_REG GPIOB->ODR
#define LED0 GPIO_ODR_ODR8
#define LED1 GPIO_ODR_ODR9
#define LED2 GPIO_ODR_ODR10
#define LED3 GPIO_ODR_ODR11
#define LED4 GPIO_ODR_ODR12
#define LED5 GPIO_ODR_ODR13
#define LED6 GPIO_ODR_ODR14
#define LED7 GPIO_ODR_ODR15

#define LED_SWAP(LED) __disable_irq();SWAP_BIT(LED_REG, LED);__enable_irq();
#define LED_ON(LED) __disable_irq();SET_BIT(LED_REG, LED);__enable_irq();
#define LED_OFF(LED) __disable_irq();CLEAR_BIT(LED_REG, LED);__enable_irq();

#define LED_IRQ_SWAP(LED) SWAP_BIT(LED_REG, LED);
#define LED_IRQ_ON(LED) SET_BIT(LED_REG, LED);
#define LED_IRQ_OFF(LED) CLEAR_BIT(LED_REG, LED);

#endif
