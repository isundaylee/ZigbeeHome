#include <stm32f1xx.h>

#include "USART.h"

#define DELAY(iterations) for (int i = 0; i < iterations; i++)

extern "C" void startup(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

  USART u(USART1);
  u.init();

  for (;;) {
    int ch;
    if ((ch = u.read()) != -1) {
      u.write((uint8_t)ch);
    }
    //   u.write('h');
    //   u.write('e');
    //   u.write('l');
    //   u.write('l');
    //   u.write('o');
    //   u.write('!');
    //
    //   DELAY(100000);
  }
}
