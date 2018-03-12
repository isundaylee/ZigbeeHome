#pragma once

#include <stm32l0xx.h>

#include "Clock.h"
#include "Interrupt.h"
#include "Tick.h"

extern "C" void vector_rtc();

static const uint32_t RTC_CALIBRATION_TICKS = 100;

class RealTimeClock {
private:
  static uint32_t ticksAtStart;
  static uint32_t ticksAtFire;
  static uint32_t ticksPerMs;

public:
  static void init() {
    static bool inited = false;

    if (inited) {
      return;
    } else {
      inited = true;
    }

    Tick::init();

    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_DBP;

    RCC->CSR |= RCC_CSR_RTCRST;
    __DSB();
    RCC->CSR &= ~RCC_CSR_RTCRST;
    __DSB();

    Clock::enableLSI();
    RCC->CSR &= ~RCC_CSR_RTCSEL_Msk;
    RCC->CSR |= RCC_CSR_RTCSEL_LSI;
    RCC->CSR |= RCC_CSR_RTCEN;

    // Calibrating the millisecond base
    ticksAtFire = 0;
    enableWakeUpTimerTicks(RTC_CALIBRATION_TICKS);
    WAIT_UNTIL(ticksAtFire != 0);
    ticksPerMs = RTC_CALIBRATION_TICKS / (ticksAtFire - ticksAtStart);
  }

  static void enableWakeUpTimer(uint32_t wucksel, uint32_t wutr) {
    Interrupt::enable();
    Interrupt::enableIRQ(RTC_IRQn);

    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    RTC->CR &= ~RTC_CR_WUTE;
    WAIT_UNTIL((RTC->ISR & RTC_ISR_WUTWF) != 0);
    RTC->CR &= ~RTC_CR_WUCKSEL_Msk;
    RTC->CR |= wucksel << RTC_CR_WUCKSEL_Pos;
    RTC->WUTR = wutr;
    RTC->CR |= (RTC_CR_WUTE | RTC_CR_WUTIE);

    ticksAtStart = Tick::value;

    RTC->WPR = 0xFF;

    EXTI->IMR |= EXTI_IMR_IM20;
    EXTI->EMR |= EXTI_EMR_EM20;
    EXTI->RTSR |= EXTI_RTSR_RT20;
  }

  static void enableWakeUpTimerSeconds(uint32_t delaySeconds) {
    enableWakeUpTimer(0b100, delaySeconds - 1);
  }

  static void enableWakeUpTimerMs(uint32_t delayMs) {
    enableWakeUpTimer(0b011, delayMs * ticksPerMs);
  }

  static void enableWakeUpTimerTicks(uint32_t delayTicks) {
    enableWakeUpTimer(0b011, delayTicks);
  }

  static void deepSleep(uint32_t wucksel, uint32_t wutr) {
    /* Enable Clocks */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

    enableWakeUpTimer(wucksel, wutr);

    PWR->CR |= PWR_CR_CWUF;    // clear the WUF flag after 2 clock cycles
    PWR->CR &= ~(PWR_CR_PDDS); // Enter stop mode when the CPU enters deepsleep
    PWR->CR |= PWR_CR_LPSDSR;
    RCC->CFGR |=
        RCC_CFGR_STOPWUCK; // HSI16 oscillator is wake-up from stop clock
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; // low-power mode = stop mode
    __WFI();                           // enter low-power mode
  }

  static void deepSleepSeconds(uint32_t delaySeconds) {
    deepSleep(0b100, delaySeconds - 1);
  }

  static void deepSleepMs(uint32_t delayMs) {
    deepSleep(0b011, delayMs * ticksPerMs);
  }

  friend void vector_rtc();
};
