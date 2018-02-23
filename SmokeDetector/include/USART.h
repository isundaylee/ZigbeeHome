#pragma once

#include <stm32l0xx.h>

#include "Clock.h"
#include "GPIO.h"
#include "Interrupt.h"
#include "RingBuffer.h"

extern "C" void vector_usart2();

template <uintptr_t usartAddr> class USART {
private:
  static_assert(usartAddr == LPUART1_BASE || usartAddr == USART2_BASE,
                "Invalid usartAddr given");

  static volatile RingBuffer<uint8_t, 64> rxBuffer_;

  static USART_TypeDef *usart() {
    return reinterpret_cast<USART_TypeDef *>(usartAddr);
  }

public:
  static void init() {
    if constexpr (usartAddr == LPUART1_BASE) {
      RCC->APB1ENR |= RCC_APB1ENR_LPUART1EN;

      GPIO_A::init();
      GPIO_A::Pin<1>::setMode(GPIO_MODE_ALTERNATE, 6);

      usart()->BRR = 4660;
    } else if constexpr (usartAddr == USART2_BASE) {
      RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

      GPIO_A::init();
      GPIO_A::Pin<9>::setMode(GPIO_MODE_ALTERNATE, 4);
      GPIO_A::Pin<10>::setMode(GPIO_MODE_ALTERNATE, 4);

      usart()->BRR = 2097000 / 115200;
    }

    // usart()->BRR = 2100000 / 115200;

    usart()->CR1 &= ~(USART_CR1_M);
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
    write(static_cast<uint8_t>((data & 0xFF000000) >> 24));
    write(static_cast<uint8_t>((data & 0x00FF0000) >> 16));
    write(static_cast<uint8_t>((data & 0x0000FF00) >> 8));
    write(static_cast<uint8_t>((data & 0x000000FF) >> 0));
  }

  static void write(const char *string) {
    for (const char *c = string; (*c) != 0; c++) {
      write(static_cast<uint8_t>(*c));
    }
  }

  static void flush() { rxBuffer_.clear(); }

  static int read() {
    uint8_t data;

    if (rxBuffer_.pop(data)) {
      return data;
    }

    return -1;
  }

  friend void vector_usart2();
};

template <uintptr_t usartAddr>
volatile RingBuffer<uint8_t, 64> USART<usartAddr>::rxBuffer_;

typedef USART<USART2_BASE> USART_2;
typedef USART<LPUART1_BASE> LPUART_1;
