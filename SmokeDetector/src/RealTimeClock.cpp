#include "RealTimeClock.h"

/* static */ uint32_t RealTimeClock::ticksAtStart = 0;
/* static */ uint32_t RealTimeClock::ticksAtFire = 0;
/* static */ uint32_t RealTimeClock::ticksPerMs = 0;

extern "C" void vector_rtc(void) {
  RealTimeClock::ticksAtFire = Tick::value;

  RTC->ISR &= ~RTC_ISR_WUTF;
  EXTI->PR |= EXTI_PR_PR20;
  WAIT_UNTIL((LPUART1->ISR & USART_ISR_TC) != 0);

  RTC->CR &= ~(RTC_CR_WUTE | RTC_CR_WUTIE);
}
