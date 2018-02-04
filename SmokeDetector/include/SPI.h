#pragma once

#include <stm32f1xx.h>

class SPI {
private:
  SPI_TypeDef *spi_;

public:
  SPI(SPI_TypeDef *spi);

  void init();
  void setupSlaveSelect(GPIO_TypeDef *gpio, int pin);

  void transfer(GPIO_TypeDef *gpioSS, int pinSS, uint8_t *buf, int length);
};
