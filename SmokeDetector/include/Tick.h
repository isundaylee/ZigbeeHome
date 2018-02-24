#pragma once

#include <stm32l011xx.h>

#include "Clock.h"
#include "Interrupt.h"

class Tick {
public:
  static volatile uint32_t value;

  static void init() {
    // TODO: Fix this when we get integer division working.
    SysTick->LOAD = 2097;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_CLKSOURCE_Msk;

    Interrupt::enable();
    Interrupt::enableIRQ(SysTick_IRQn);
  }

  static void delay(int ms) {
    uint32_t target = value + ms;

    while (value < target)
      asm("nop");
  }

  static bool hasElapsedSince(uint32_t since, uint32_t duration) {
    return (value >= since + duration);
  }
};
