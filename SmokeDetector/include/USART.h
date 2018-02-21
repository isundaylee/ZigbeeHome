#pragma once

#include <stm32l0xx.h>

#include "GPIO.h"
#include "RingBuffer.h"
#include "Interrupt.h"

extern "C" void vector_usart2();

template <int usartAddr> class USART {
private:
  static volatile RingBuffer<uint8_t, 64> rxBuffer_;

  static USART_TypeDef *usart() { return (USART_TypeDef *)usartAddr; }

public:
  static void init() {
    if (usartAddr == USART2_BASE) {
      RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

      GPIO_A::init();
      GPIO_A::Pin<9>::setMode(GPIO_MODE_ALTERNATE, 0b0100);
      GPIO_A::Pin<9>::setMode(GPIO_MODE_ALTERNATE, 0b0100);
    }

    usart()->CR1 &= ~(USART_CR1_M);
    usart()->BRR = 2100000 / 115200;
    usart()->CR1 |= USART_CR1_RXNEIE;
    usart()->CR1 |= USART_CR1_UE;
    usart()->CR1 |= USART_CR1_TE;
    usart()->CR1 |= USART_CR1_RE;

    Interrupt::enable();
    Interrupt::enableIRQ(USART2_IRQn);
  }

  static void write(uint8_t data) {
    while ((usart()->ISR & USART_ISR_TXE) == 0)
      ;

    usart()->TDR = data;
  }

  static void write(uint32_t data) {
    write((uint8_t)((data & 0xFF000000) >> 24));
    write((uint8_t)((data & 0x00FF0000) >> 16));
    write((uint8_t)((data & 0x0000FF00) >> 8));
    write((uint8_t)((data & 0x000000FF) >> 0));
  }

  static void write(const char *string) {
    for (const char *c = string; (*c) != 0; c++) {
      write((uint8_t)*c);
    }
  }

  static int read() {
    uint8_t data;

    if (rxBuffer_.pop(data)) {
      return data;
    }

    return -1;
  }

  friend void vector_usart2();
};

typedef USART<USART2_BASE> USART_2;
template <int usartAddr> volatile RingBuffer<uint8_t, 64> USART<usartAddr>::rxBuffer_;
