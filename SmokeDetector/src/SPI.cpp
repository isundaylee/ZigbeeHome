#include "SPI.h"

#include <unistd.h>

SPI::SPI(SPI_TypeDef *spi) : spi_(spi) {}

void SPI::init() {
  if (spi_ == SPI1) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // SCLK on A5
    GPIOA->CRL &= ~(0b1111UL << 20);
    GPIOA->CRL |= (0b1010UL << 20);

    // MOSI on A7
    GPIOA->CRL &= ~(0b1111UL << 28);
    GPIOA->CRL |= (0b1010UL << 28);
  }

  SPI1->CR1 &= ~(0b111 << 3);
  SPI1->CR1 |= (0b100 << 3);
  SPI1->CR2 |= SPI_CR1_SSI;
  SPI1->CR2 |= SPI_CR1_SSM;
  SPI1->CR1 |= SPI_CR1_MSTR;
  SPI1->CR1 |= SPI_CR1_SPE;
}

void SPI::setupSlaveSelect(GPIO_TypeDef *gpio, int pin) {
  gpio->CRL &= ~(0b1111UL << (4 * pin));
  gpio->CRL |= (0b0001UL << (4 * pin));
}

void SPI::transfer(GPIO_TypeDef *gpioSS, int pinSS, uint8_t *buf, int length) {
  gpioSS->BSRR = (1 << (16 + pinSS));

  for (size_t i = 0; i < length; i++) {
    SPI1->DR = buf[i];
    while ((SPI1->SR & SPI_SR_RXNE) == 0)
      ;
    buf[i] = SPI1->DR;
  }

  gpioSS->BSRR = (1 << (0 + pinSS));
}
