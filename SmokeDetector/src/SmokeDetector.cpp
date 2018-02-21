#include <stm32l011xx.h>

#include "GPIO.h"
#include "USART.h"
#include "Zigbee.h"

void setupClock() {}

typedef GPIO_A::Pin<4> LEDPin;

void notmain(void) {
  setupClock();

  LEDPin::GPIO::init();
  LEDPin::setMode(GPIO_MODE_OUTPUT);
  LEDPin::clear();

  Zigbee<USART_2, GPIO_A::Pin<1>> bee;
  bee.reset();

  while (true) {
    LEDPin::set(bee.isPowered);
    bee.process();
  }
}
