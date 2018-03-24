#include <stm32l011xx.h>

#include "ADC.h"
#include "Clock.h"
#include "GPIO.h"
#include "RealTimeClock.h"
#include "SimpleZigbee.h"
#include "Tick.h"
#include "USART.h"
#include "Utils.h"

typedef GPIO_A::Pin<7> InitPin;

typedef GPIO_A::Pin<4> LEDPin;

typedef GPIO_C::Pin<14> ZigbeeResetPin;
typedef GPIO_C::Pin<15> ZigbeeWakeUpPin;
typedef SimpleZigbee<USART_2, ZigbeeResetPin, ZigbeeWakeUpPin> MyZigbee;

const static uint32_t CONNECTION_TIMEOUT = 15000;

extern "C" void main(void) {
  Clock::enableHSI();
  Clock::selectSystemClock(RCC_CFGR_SW_HSI);
  RealTimeClock::init();
  Tick::init();

  DebugUART::init();
  DebugPrint("[Main]   Hello, world!\n");

  InitPin::GPIO::init();
  InitPin::setMode(GPIO_MODE_INPUT);
  InitPin::setPullMode(GPIO_PULL_UP);

  MyZigbee bee(ZIGBEE_ROLE_COORDINATOR, 0xBEEF);
  bee.init();

  bool shouldInit = !InitPin::get();

  while (true) {
    if (!bee.isConnected()) {
      // Connect if we're not connected (or disconnected)

      if (shouldInit) {
        DebugPrint("[Main]   Forming network...\n");
      } else {
        DebugPrint("[Main]   Restoring...\n");
      }

      if (bee.connect(shouldInit, CONNECTION_TIMEOUT)) {
        shouldInit = false;
        DebugPrint("[Main]   Connect successful!\n");
      } else {
        DebugPrint("[Main]   Connect failed!\n");
      }
    }

    bee.bee.process();
  }
}
