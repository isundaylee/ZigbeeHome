#include "USART.h"

extern "C" void __aeabi_unwind_cpp_pr0(void) {}
extern "C" void __aeabi_unwind_cpp_pr1(void) {}

extern "C" void vector_usart2() {
  while (USART_2.usart_->ISR & USART_ISR_RXNE) {
    USART_2.rxBuffer_.push((uint8_t)USART_2.usart_->RDR);
  }
}

USART USART_2(USART2);

USART::USART(USART_TypeDef *usart) : usart_(usart) {}

void USART::init() {
  if (usart_ == USART2) {
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    RCC->IOPENR |= RCC_IOPENR_IOPAEN;

    GPIOA->MODER &= ~(0b11 << 18);
    GPIOA->MODER |= (0b10 << 18);
    GPIOA->AFR[1] |= (0b0100 << 4);

    GPIOA->MODER &= ~(0b11 << 20);
    GPIOA->MODER |= (0b10 << 20);
    GPIOA->AFR[1] |= (0b0100 << 8);
  }

  usart_->CR1 &= ~(USART_CR1_M);
  usart_->BRR = 2100000 / 115200;
  usart_->CR1 |= USART_CR1_RXNEIE;
  usart_->CR1 |= USART_CR1_UE;
  usart_->CR1 |= USART_CR1_TE;
  usart_->CR1 |= USART_CR1_RE;
}

void USART::write(uint8_t data) {
  while ((usart_->ISR & USART_ISR_TXE) == 0)
    ;

  usart_->TDR = data;
}

void USART::write(uint32_t data) {
  this->write((uint8_t)((data & 0xFF000000) >> 24));
  this->write((uint8_t)((data & 0x00FF0000) >> 16));
  this->write((uint8_t)((data & 0x0000FF00) >> 8));
  this->write((uint8_t)((data & 0x000000FF) >> 0));
}

void USART::write(const char *string) {
  for (const char *c = string; (*c) != 0; c++) {
    this->write((uint8_t)*c);
  }
}

int USART::read() {
  uint8_t data;

  if (rxBuffer_.pop(data)) {
    return data;
  }

  return -1;

  // if ((usart_->ISR & USART_ISR_RXNE) != 0) {
  //   return (uint8_t)usart_->RDR;
  // }
  //
  // return -1;
}
