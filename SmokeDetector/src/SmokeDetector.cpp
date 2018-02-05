#include <stm32f1xx.h>

#include "SPI.h"
#include "USART.h"

#define DELAY(iterations) for (int i = 0; i < iterations; i++)

void setupClock() {
  while ((RCC->CR & RCC_CR_HSIRDY) == 0)
    ;

  RCC->CFGR |= (0b0111 << 18);
  RCC->CR |= RCC_CR_PLLON;
  RCC->CFGR |= 0b10;
}

extern "C" void startup(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

  setupClock();

  SPI s(SPI1);
  s.init();
  s.setupSlaveSelect(GPIOA, 2);

  for (int count = 0;; count++) {
    uint8_t buf[] = {0b1101, 0b0, 0b0};

    s.transfer(GPIOA, 2, buf, 3);

    uint16_t value = (((uint16_t)buf[1]) << 3) + (buf[2] & 0b111);
    uint8_t v = (value >> 2);

    DELAY(100000);
  }
}
