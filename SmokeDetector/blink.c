#include <stm32f1xx.h>

#define DELAY(cycles) for (int i = 0; i < cycles; i++)

void delay(void) {
  volatile int count = 1000 * 200;

  while (count--)
    ;
}

void bitbang(uint8_t value) {
  GPIOB->ODR = 0;
  DELAY(20);

  for (uint8_t bit = 0x80; bit != 0; bit >>= 1) {
    if ((value & bit) == 0) {
      GPIOB->ODR = 0;
    } else {
      GPIOB->ODR = 1 << 5;
    }

    DELAY(4);
  }

  GPIOB->ODR = 0;
  DELAY(20);
}

void startup(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

  GPIOA->CRH |= (0b1001 << 4);

  USART1->CR1 |= USART_CR1_UE;
  USART1->CR1 &= ~(USART_CR1_M);
  USART1->BRR = 0x040 + 0x6;
  USART1->CR1 |= USART_CR1_TE;

  for (;;) {
    while ((USART1->SR & USART_SR_TXE) == 0)
      ;

    USART1->DR = 0x55;
  }
}
