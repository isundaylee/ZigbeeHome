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
  DebugPrint("[Main]   Bee came to life!\n");

  if (bee.resetSettings()) {
    DebugPrint("[Main]   Reset successful!\n");
  } else {
    DebugPrint("[Main]   Reset failed...\n");
    return false;
  }

  if (bee.setRole(ZIGBEE_ROLE_ROUTER)) {
    DebugPrint("[Main]   Setting role was successful!\n");
  } else {
    DebugPrint("[Main]   Setting role failed...\n");
    return false;
  }

  if (bee.setChannelMask(true, 0x00002000)) {
    DebugPrint("[Main]   Setting primary channel mask was successful!\n");
  } else {
    DebugPrint("[Main]   Setting primary channel mask failed...\n");
    return false;
  }

  if (bee.setChannelMask(false, 0x00002000)) {
    DebugPrint("[Main]   Setting primary channel mask was successful!\n");
  } else {
    DebugPrint("[Main]   Setting primary channel mask failed...\n");
    return false;
  }

  if (bee.startCommissioning(ZIGBEE_COMMISSIONING_MODE_NETWORK_STEERING)) {
    DebugPrint("[Main]   Starting network steering was successful!\n");
  } else {
    DebugPrint("[Main]   Starting network steering failed...\n");
    return false;
  }

  uint16_t clusters[] = {0x0000, 0x0006};
  if (bee.registerEndpoint(0x01, 0x0104, 0x0100, 0x00, 2, clusters, 2,
                           clusters)) {
    DebugPrint("[Main]   Registration was successful!\n");
  } else {
    DebugPrint("[Main]   Registration failed...\n");
    return false;
  }

  return true;
}

void notmain(void) {
  setupClock();

  LEDPin::GPIO::init();
  LEDPin::setMode(GPIO_MODE_OUTPUT, 0);

  DebugUART::init();
  DebugPrint("[Main]   Hello, world!\n");

  LEDPin::GPIO::init();
  LEDPin::setMode(GPIO_MODE_OUTPUT);
  LEDPin::clear();

  MyZigbee bee;
  setupBeeRouter(bee);

  while (bee.zdoState != ZIGBEE_ZDO_STATE_ROUTER)
    bee.process();

  uint8_t reqData[] = {0x11, 0x22, 0x33};

  while (true) {
    if (bee.dataRequest(0x0000, 0x01, 0x01, 0x0006, 0x00, 0x08, 0x0F,
                        sizeof(reqData), reqData)) {
      DebugPrint("[Main]   Data request was successful!\n");
    } else {
      DebugPrint("[Main]   Data request failed...\n");
    }

    DELAY(200000);
  }

  while (true) {
    LEDPin::set(bee.zdoState == ZIGBEE_ZDO_STATE_ROUTER);
    bee.process();
  }
}
