#pragma once

#include <stm32l0xx.h>

#include "RingBuffer.h"

extern "C" void vector_usart2();

class USART {
private:
  USART_TypeDef *usart_;
  RingBuffer<uint8_t, 64> rxBuffer_;

public:
  USART(USART_TypeDef *usart);

  void init();

  void write(uint8_t data);
  void write(uint32_t data);
  void write(const char *string);
  int read();

  friend void vector_usart2();
};

extern USART USART_2;
