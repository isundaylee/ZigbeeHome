#include <stm32f1xx.h>

#define MODE_OUT_2 0x02 /* Output, 2 Mhz */

#define CONF_GP_UD 0x0 /* Pull up/down */
#define CONF_GP_OD 0x4 /* Open drain */

void delay(void) {
  volatile int count = 1000 * 200;

  while (count--)
    ;
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
  }
}
