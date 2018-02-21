#include <stm32l011xx.h>

#include "GPIO.h"
#include "Interrupt.h"
#include "USART.h"

void setupClock() {}

void notmain(void) {
  setupClock();

  Interrupt::enable();
  Interrupt::enableIRQ(USART2_IRQn);

  USART_2.init();
  USART_2.write("Hello, world!");

  GPIO_A::init();
  GPIO_A::setMode(4, GPIO_MODE_OUTPUT);

  while (true) {
    int c;
    while ((c = USART_2.read()) != -1) {
      USART_2.write((uint8_t)c);
    }
  }
}

extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint8_t __data_start__;
extern uint8_t __data_start_load__;
extern uint8_t __data_end__;

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
