#include <stm32f1xx.h>

#include "USART.h"

#define DELAY(iterations) for (int i = 0; i < iterations; i++)

extern "C" void startup(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

  USART u(USART1);
  u.init();

  for (;;) {
    u.write(0xAB);
    u.write(0xCD);
    u.write(0xEF);

    DELAY(1000);
  }
}
