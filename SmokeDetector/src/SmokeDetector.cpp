#include <stm32l011xx.h>

#include "Clock.h"
#include "GPIO.h"
#include "USART.h"
#include "Zigbee.h"

void setupClock() {
  // Clock::setClockOut(RCC_CFGR_MCOSEL_SYSCLK);
  // GPIO_A::init();
  // GPIO_A::Pin<9>::setMode(GPIO_MODE_ALTERNATE, 0);
  //
  // Clock::setMSIRange(RCC_ICSCR_MSIRANGE_4);
}

typedef GPIO_A::Pin<4> LEDPin;

void notmain(void) {
  setupClock();

  LEDPin::GPIO::init();
  LEDPin::setMode(GPIO_MODE_OUTPUT, 0);

  DebugUART::init();
  DebugPrint("Hello, world!");

  while (true) {
    LEDPin::set();
    DELAY(20000);
    LEDPin::clear();
    DELAY(20000);
  }

  DELAY(10000000);
  //
  //
  // while (true) {
  //   GPIO_A::Pin<4>::set();
  //   DELAY(10000);
  //   GPIO_A::Pin<4>::clear();
  // }
  //
  // DELAY(10000000);

  // LEDPin::GPIO::init();
  // LEDPin::setMode(GPIO_MODE_OUTPUT);
  // LEDPin::clear();
  //
  // Zigbee<USART_2, GPIO_A::Pin<1>> bee;
  // bee.reset();
  //
  // while (true) {
  //   LEDPin::set(bee.isPowered);
  //   bee.process();
  // }
}
