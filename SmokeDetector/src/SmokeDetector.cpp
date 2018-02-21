#include <stm32l011xx.h>

#include "GPIO.h"
#include "USART.h"

void setupClock() {}

void notmain(void) {
  setupClock();

  USART_2::init();
  USART_2::write("Hello, world!");

  GPIO_A::init();
  GPIO_A::Pin<1>::setMode(GPIO_MODE_OUTPUT);
  GPIO_A::Pin<4>::setMode(GPIO_MODE_OUTPUT);

  GPIO_A::Pin<1>::set();

  while (true) {
    int c;
    while ((c = USART_2::read()) != -1) {
      USART_2::write((uint8_t)c);
    }
  }
}

extern uint32_t __attribute__((may_alias)) __bss_start__;
extern uint32_t __attribute__((may_alias)) __bss_end__;
extern uint8_t __attribute__((may_alias)) __data_start__;
extern uint8_t __attribute__((may_alias)) __data_start_load__;
extern uint8_t __attribute__((may_alias)) __data_end__;

extern "C" void startup(void) {
  // Zero the BSS section
  for (uint32_t *p = &__bss_start__; p != &__bss_end__; p++) {
    *p = 0;
  }

  // Copy the DATA section
  for (uint8_t *dst = &__data_start__, *src = &__data_start_load__;
       dst != &__data_end__; dst++, src++) {
    *dst = *src;
  }

  // Calling global initializers
  extern void (*__init_array_start__)();
  extern void (*__init_array_end__)();

  for (void (**p)() = &__init_array_start__; p < &__init_array_end__; p++) {
    (*p)();
  }

  notmain();
}
