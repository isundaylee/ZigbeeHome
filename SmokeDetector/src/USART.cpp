#include "USART.h"

extern "C" void __aeabi_unwind_cpp_pr0(void) {}
extern "C" void __aeabi_unwind_cpp_pr1(void) {}

USART::USART(USART_TypeDef *usart) : usart_(usart) {}

void USART::init() {
  if (usart_ == USART1) {
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRH |= (0b1001 << 4);
  }

  usart_->CR1 |= USART_CR1_UE;
  usart_->CR1 &= ~(USART_CR1_M);
  usart_->BRR = 0x040 + 0x6;
  usart_->CR1 |= USART_CR1_TE;
  usart_->CR1 |= USART_CR1_RE;
}

void USART::write(uint8_t data) {
  while ((usart_->SR & USART_SR_TXE) == 0)
    ;

  usart_->DR = data;
}

int USART::read() {
  if ((usart_->SR & USART_SR_RXNE) != 0) {
    return (uint8_t)usart_->DR;
  }

  return -1;
}
