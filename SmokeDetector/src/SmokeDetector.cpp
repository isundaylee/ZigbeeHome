#include <stm32f1xx.h>

#include "SPI.h"
#include "USART.h"

#define DELAY(iterations) for (int i = 0; i < iterations; i++)

void setupClock() {
  while ((RCC->CR & RCC_CR_HSIRDY) == 0)
    ;

  RCC->CFGR |= (0b0110 << 18);
  RCC->CR |= RCC_CR_PLLON;
  RCC->CFGR |= 0b10;
}

extern "C" void startup(void) {
  setupClock();

  USART u(USART1);
  u.init();

  SPI s(SPI1);
  s.init();
  s.setupSlaveSelect(GPIOA, 2);

  u.write("Hello!");

  for (int count = 0;; count++) {
    uint8_t buf[] = {0b1101, 0b0, 0b0};

    s.transfer(GPIOA, 2, buf, 3);

    uint16_t value = (((uint16_t)buf[1]) << 3) + ((buf[2] & 0b11100000) >> 5);
    uint8_t v = (value >> 2);

    u.write(buf[1]);

    DELAY(100000);
  }
}
