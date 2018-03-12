#pragma once

#include <stm32l0xx.h>

#define WAIT_UNTIL(condition)                                                  \
  do {                                                                         \
    asm("nop");                                                                \
  } while (!(condition))

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

  static void enableLSI() {
    RCC->CSR |= RCC_CSR_LSION;
    WAIT_UNTIL((RCC->CSR & RCC_CSR_LSIRDY) != 0);
  }

  static void enableHSI() {
    RCC->CR |= RCC_CR_HSION;
    WAIT_UNTIL((RCC->CR & RCC_CR_HSIRDY) != 0);
  }

  static void selectSystemClock(uint32_t systemClock) {
    RCC->CFGR &= ~RCC_CFGR_SW_Msk;
    RCC->CFGR |= systemClock;

    switch (systemClock) {
    case RCC_CFGR_SW_HSI:
      currentClockFrequency = 16000000;
    }
  }
};
