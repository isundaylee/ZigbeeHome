#pragma once

#include <stm32l0xx.h>

class Clock {
public:
  static uint32_t currentClockFrequency;

  static void setClockOut(uint32_t clockOut) {
    RCC->CFGR &= ~RCC_CFGR_MCOSEL_Msk;
    RCC->CFGR |= clockOut;
  }

  static void setMSIRange(uint32_t msiRange) {
    RCC->ICSCR &= ~RCC_ICSCR_MSIRANGE_Msk;
    RCC->ICSCR |= msiRange;

    switch (msiRange) {
    case RCC_ICSCR_MSIRANGE_0:
      currentClockFrequency = 65536;
      break;
    case RCC_ICSCR_MSIRANGE_1:
      currentClockFrequency = 131072;
      break;
    case RCC_ICSCR_MSIRANGE_2:
      currentClockFrequency = 262144;
      break;
    case RCC_ICSCR_MSIRANGE_3:
      currentClockFrequency = 524288;
      break;
    case RCC_ICSCR_MSIRANGE_4:
      currentClockFrequency = 1048000;
      break;
    case RCC_ICSCR_MSIRANGE_5:
      currentClockFrequency = 2097000;
      break;
    case RCC_ICSCR_MSIRANGE_6:
      currentClockFrequency = 4194000;
      break;
    }
  }
};
