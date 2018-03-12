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
const static uint32_t CONNECTION_FAILURE_TIMEOUT_SECONDS = 10;

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

  MyZigbee bee(ZIGBEE_ROLE_END_DEVICE, 0xBEEF);
  bee.init();

  ADC_1::init();
  ADC_1::enableVoltageReference();
  ADC_1::selectChannel(ADC_CHANNEL_VOLTAGE_REFERENCE);

  bool shouldInit = !InitPin::get();

  while (true) {
    if (!bee.isConnected()) {
      // Connect if we're not connected (or disconnected)

      if (shouldInit) {
        DebugPrint("[Main]   Setting up...\n");
      } else {
        DebugPrint("[Main]   Connecting...\n");
      }

      if (bee.connect(shouldInit, CONNECTION_TIMEOUT)) {
        shouldInit = false;
        DebugPrint("[Main]   Connect successful!\n");
      } else {
        DebugPrint("[Main]   Connect failed!\n");
        DebugUART::waitUntilTxDone();
        bee.bee.turnOff();
        RealTimeClock::deepSleepSeconds(CONNECTION_FAILURE_TIMEOUT_SECONDS);
        continue;
      }
    }

    uint32_t voltage = 3 * 4096 * ADC_1::getVoltageReferenceCalibrationValue() /
                       ADC_1::convert();
    uint8_t data[] = {static_cast<uint8_t>((voltage & 0xFF00) >> 8),
                      static_cast<uint8_t>(voltage & 0x00FF)};
    bee.send(0x0000, sizeof(data), data);

    DebugUART::waitUntilTxDone();
    RealTimeClock::deepSleepMs(1000);
  }
}
