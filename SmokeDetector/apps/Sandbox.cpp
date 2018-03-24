#include <stm32l011xx.h>

#include "ADC.h"
#include "Clock.h"
#include "Tick.h"
#include "Utils.h"

extern "C" void main(void) {
  Clock::enableHSI();
  Clock::selectSystemClock(RCC_CFGR_SW_HSI);
  Tick::init();

  ADC_1::init();
  ADC_1::enableVoltageReference();

  DebugUART::init();

  while (true) {
    DebugPrint("\nHello, world! ");
    Tick::delay(1000);

    for (int i = 0; i < 60; i++) {
      DebugPrint(".");
      Tick::delay(100);
    }
  }
}
