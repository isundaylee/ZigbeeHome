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

void notmain(void) {
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

  USART_2.init();
  USART_2.write("Hello, world!");

  while (true) {
    int c;
    while ((c = USART_2.read()) != -1)
      USART_2.write((uint8_t)c);
    //   GPIOA->ODR |= (1 << 4);
    //   u.write((uint8_t)0x55);
    //   GPIOA->ODR &= ~(1 << 4);
    //
    //   DELAY(1000);
  }
}

extern uint32_t __bss_start__;
extern uint32_t __bss_end__;

extern "C" void startup(void) {
  // Zero the BSS section
  for (uint32_t *p = &__bss_start__; p != &__bss_end__; p++) {
    *p = 0;
  }

  // Calling global initializers
  extern void (*__init_array_start__)();
  extern void (*__init_array_end__)();

  for (void (**p)() = &__init_array_start__; p < &__init_array_end__; p++) {
    (*p)();
  }

  notmain();
}
