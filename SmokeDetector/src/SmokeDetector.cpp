#include <stm32l011xx.h>

#include "ADC.h"
#include "Clock.h"
#include "GPIO.h"
#include "SimpleZigbee.h"
#include "Tick.h"
#include "USART.h"
#include "Utils.h"

typedef GPIO_A::Pin<4> LEDPin;
typedef SimpleZigbee<USART_2, GPIO_A::Pin<7>> MyZigbee;

extern "C" void main(void) {
  Tick::init();

  DebugUART::init();
  DebugPrint("[Main]   Hello, world!\n");

  LEDPin::GPIO::init();
  LEDPin::setMode(GPIO_MODE_OUTPUT);
  LEDPin::clear();

  MyZigbee bee(ZIGBEE_ROLE_ROUTER, 0xBEEF);
  bee.init();

  bee.connect(true, 10000);

  while (true) {
    if (!bee.isConnected()) {
      LEDPin::clear();

      // Connect if we're not connected (or disconnected)
      if (bee.connect(false, 10000)) {
        DebugPrint("[Main]   Connect successful!\n");
        LEDPin::set();
      } else {
        DebugPrint("[Main]   Connect failed!\n");
        Tick::delay(1000);
        continue;
      }
    }

    uint8_t data[] = {0x11, 0x22, 0x33};
    bee.send(0x0000, sizeof(data), data);
    // Tick::delay(300);
  }
}
