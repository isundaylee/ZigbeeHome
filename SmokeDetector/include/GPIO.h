#pragma once

#include <stm32l0xx.h>

const uint32_t GPIO_MODE_INPUT = 0b00;
const uint32_t GPIO_MODE_OUTPUT = 0b01;
const uint32_t GPIO_MODE_ALTERNATE = 0b10;
const uint32_t GPIO_MODE_ANALOG = 0b11;

const uint32_t GPIO_PULL_NONE = 0b00;
const uint32_t GPIO_PULL_UP = 0b01;
const uint32_t GPIO_PULL_DOWN = 0b10;

template <uintptr_t gpioAddr> class GPIO {
private:
  static_assert((gpioAddr == GPIOA_BASE) || (gpioAddr == GPIOB_BASE) ||
                    (gpioAddr == GPIOC_BASE),
                "Invalid gpioAddr given.");

  static GPIO_TypeDef *gpio() {
    return reinterpret_cast<GPIO_TypeDef *>(gpioAddr);
  }

public:
  static void init() {
    static bool inited = false;

    if (inited) {
      return;
    }

    inited = true;

    if constexpr (gpioAddr == GPIOA_BASE) {
      RCC->IOPENR |= RCC_IOPENR_IOPAEN;
    } else if constexpr (gpioAddr == GPIOB_BASE) {
      RCC->IOPENR |= RCC_IOPENR_IOPBEN;
    } else if constexpr (gpioAddr == GPIOC_BASE) {
      RCC->IOPENR |= RCC_IOPENR_IOPCEN;
    }
  }

  template <int pin> class Pin {
  public:
    typedef GPIO<gpioAddr> GPIO;

    static void setMode(uint32_t mode, uint32_t alternate = 0) {
      gpio()->MODER &= ~(0b11 << (2 * pin));
      gpio()->MODER |= (mode << (2 * pin));
      gpio()->AFR[pin / 8] &= ~(0b1111 << (4 * (pin % 8)));
      gpio()->AFR[pin / 8] |= (alternate << (4 * (pin % 8)));
    }

    static void setPullMode(uint32_t mode) {
      gpio()->PUPDR &= ~(0b11 << (2 * pin));
      gpio()->PUPDR |= (mode << (2 * pin));
    }

    static void set() { gpio()->BSRR = (1UL << pin); }
    static void clear() { gpio()->BSRR = (1UL << (pin + 16)); }
    static void toggle() { gpio()->ODR ^= (1UL << pin); }
    static bool get() { return (gpio()->IDR & (1UL << pin)) != 0; }

    static void set(bool value) {
      if (value) {
        set();
      } else {
        clear();
      }
    }
  };
};

typedef GPIO<GPIOA_BASE> GPIO_A;
typedef GPIO<GPIOB_BASE> GPIO_B;
typedef GPIO<GPIOC_BASE> GPIO_C;
