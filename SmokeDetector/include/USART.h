#pragma once

#include <stm32f1xx.h>

class USART {
private:
  USART_TypeDef *usart_;

public:
  USART(USART_TypeDef *usart);

  void init();

  void write(uint8_t data);
  int read();
};
