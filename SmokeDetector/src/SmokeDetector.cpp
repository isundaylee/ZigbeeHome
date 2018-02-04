#include <stm32f1xx.h>

#include "SPI.h"
#include "USART.h"

#define DELAY(iterations) for (int i = 0; i < iterations; i++)

extern "C" void startup(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

  USART u(USART1);
  u.init();

  SPI s(SPI1);
  s.init();
  s.setupSlaveSelect(GPIOA, 2);

  u.write("Hello!\n");

  for (int count = 0;; count++) {
    uint8_t buf[] = {0b1101, 0b0, 0b0};

    s.transfer(GPIOA, 2, buf, 3);

    uint16_t value = (((uint16_t)buf[1]) << 3) + (buf[2] & 0b111);
    uint8_t v = (value >> 2);

    if (count % 10000 == 0) {
      u.write((uint8_t) 0x00);
    }
  }
}
