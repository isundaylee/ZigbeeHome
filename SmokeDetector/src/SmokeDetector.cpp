#include <stm32l011xx.h>

#include "USART.h"

#define DELAY(iterations) for (int i = 0; i < iterations; i++)

void setupClock() {
  // while ((RCC->CR & RCC_CR_HSIRDY) == 0)
  //   ;
  //
  // RCC->CFGR |= (0b0110 << 18);
  // RCC->CR |= RCC_CR_PLLON;
  // RCC->CFGR |= 0b10;
}

extern "C" void startup(void) {
  setupClock();

  // RCC->IOPENR |= RCC_IOPENR_IOPAEN;

  // GPIOA->MODER |= (0b01 << 8);
  //
  // while (true) {
  //   GPIOA->ODR |= (1 << 4);
  //   // DELAY(1);
  //   GPIOA->ODR &= ~(1 << 4);
  //   // DELAY(1);
  // }

  GPIOA->MODER |= (0b01 << 8);

  USART u(USART2);
  u.init();
  u.write("Hello, world!");

  while (true) {
    int c;
    while ((c = u.read()) != -1)
      u.write((uint8_t)c);
    //   GPIOA->ODR |= (1 << 4);
    //   u.write((uint8_t)0x55);
    //   GPIOA->ODR &= ~(1 << 4);
    //
    //   DELAY(1000);
  }
}
