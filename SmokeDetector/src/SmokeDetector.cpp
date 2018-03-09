#include <stm32l011xx.h>

#include "ADC.h"
#include "Clock.h"
#include "GPIO.h"
#include "SimpleZigbee.h"
#include "Tick.h"
#include "USART.h"
#include "Utils.h"

typedef GPIO_A::Pin<4> LEDPin;
typedef SimpleZigbee<USART_2, GPIO_C::Pin<14>, GPIO_C::Pin<15>> MyZigbee;
typedef GPIO_A::Pin<7> InitPin;

extern "C" void main(void) {
  Tick::init();

  DebugUART::init();
  DebugPrint("[Main]   Hello, world!\n");

  InitPin::GPIO::init();
  InitPin::setMode(GPIO_MODE_INPUT);
  InitPin::setPullMode(GPIO_PULL_UP);

  MyZigbee bee(ZIGBEE_ROLE_END_DEVICE, 0xBEEF);
  bee.init();

  // Should we initialize?
  if (!InitPin::get()) {
    bee.connect(true, 10000);
  }

  ADC_1::init();
  ADC_1::enableVoltageReference();
  ADC_1::selectChannel(ADC_CHANNEL_VOLTAGE_REFERENCE);

  while (true) {
    if (!bee.isConnected()) {
      // Connect if we're not connected (or disconnected)
      DebugPrint("[Main]   Connecting...\n");
      if (bee.connect(false, 10000)) {
        DebugPrint("[Main]   Connect successful!\n");
      } else {
        DebugPrint("[Main]   Connect failed!\n");
        bee.bee.turnOff();
        Tick::delay(5000);
        continue;
      }
    }

    uint32_t voltage = 3 * 4096 * ADC_1::getVoltageReferenceCalibrationValue() /
                       ADC_1::convert();
    uint8_t data[] = {static_cast<uint8_t>((voltage & 0xFF00) >> 8),
                      static_cast<uint8_t>(voltage & 0x00FF)};
    bee.send(0x0000, sizeof(data), data);
    // bee.bee.permitJoiningRequest(0xFFFC, 0xFE);
    Tick::delay(5000);
  }
}
