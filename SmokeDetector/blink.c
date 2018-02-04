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
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

  GPIOB->CRL |= 0b0101 << 20;

  for (;;) {
    GPIOB->BSRR = 1 << 5;
    __asm__ ( "nop");
    __asm__ ( "nop");
    __asm__ ( "nop");
    __asm__ ( "nop");
    GPIOB->BSRR = 1 << (5 + 16);
    __asm__ ( "nop");
    __asm__ ( "nop");
    __asm__ ( "nop");
    __asm__ ( "nop");

    // bitbang(0x77);
  }
}
