#include <stm32l011xx.h>

#include "Clock.h"
#include "GPIO.h"
#include "USART.h"
#include "Zigbee.h"

void setupClock() {}

typedef GPIO_A::Pin<4> LEDPin;
typedef Zigbee<USART_2, GPIO_A::Pin<7>> MyZigbee;

bool setupBeeRouter(MyZigbee &bee) {
  bee.init();
  bee.reset();
  DebugPrint("Bee came to life!\n");

  if (bee.resetSettings()) {
    DebugPrint("Reset successful!\n");
  } else {
    DebugPrint("Reset failed...\n");
    return false;
  }

  if (bee.setRole(ZIGBEE_ROLE_ROUTER)) {
    DebugPrint("Setting role was successful!\n");
  } else {
    DebugPrint("Setting role failed...\n");
    return false;
  }

  if (bee.setChannelMask(true, 0x00002000)) {
    DebugPrint("Setting primary channel mask was successful!\n");
  } else {
    DebugPrint("Setting primary channel mask failed...\n");
    return false;
  }

  if (bee.setChannelMask(false, 0x00002000)) {
    DebugPrint("Setting primary channel mask was successful!\n");
  } else {
    DebugPrint("Setting primary channel mask failed...\n");
    return false;
  }

  if (bee.startCommissioning(ZIGBEE_COMMISSIONING_MODE_NETWORK_STEERING)) {
    DebugPrint("Starting network steering was successful!\n");
  } else {
    DebugPrint("Starting network steering failed...\n");
    return false;
  }

  return true;
}

void notmain(void) {
  setupClock();

  LEDPin::GPIO::init();
  LEDPin::setMode(GPIO_MODE_OUTPUT, 0);

  DebugUART::init();
  DebugPrint("Hello, world!\n");

  LEDPin::GPIO::init();
  LEDPin::setMode(GPIO_MODE_OUTPUT);
  LEDPin::clear();

  MyZigbee bee;
  setupBeeRouter(bee);

  while (true) {
    LEDPin::set(bee.zdoState == ZIGBEE_ZDO_STATE_ROUTER);
    bee.process();
  }
}
