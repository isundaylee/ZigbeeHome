#include <stm32l011xx.h>

#include "Clock.h"
#include "GPIO.h"
#include "Tick.h"
#include "USART.h"
#include "Zigbee.h"

void setupClock() {}

typedef GPIO_A::Pin<4> LEDPin;
typedef Zigbee<USART_2, GPIO_A::Pin<7>> MyZigbee;

bool report(const char *action, bool result) {
  DebugPrint("[Main]   ");
  DebugPrint(action);
  if (result) {
    DebugPrint(" succeeded! \n");
  } else {
    DebugPrint(" failed... \n");
  }

  return result;
}

void setupBee(MyZigbee &bee, uint8_t role, bool reset = false) {
  bee.init();
  DebugPrint("[Main]   Bee came to life!\n");

  if (reset) {
    report("Reset", bee.resetSettings());
    report("Setting role", bee.setRole(role));
    report("Setting primary channel", bee.setChannelMask(true, 0x00002000));
    report("Setting secondary channel", bee.setChannelMask(false, 0x00002000));
  }

  uint16_t clusters[] = {0x0000, 0x0006};
  report("Registration", bee.registerEndpoint(0x01, 0x0104, 0x0100, 0x00, 2,
                                              clusters, 2, clusters));
  report("Starting formation", bee.startup());
  while (!bee.isOnline)
    bee.process();
}

void notmain(void) {
  setupClock();

  Tick::init();

  DebugUART::init();
  DebugPrint("[Main]   Hello, world!\n");

  LEDPin::GPIO::init();
  LEDPin::setMode(GPIO_MODE_OUTPUT);
  LEDPin::clear();

  MyZigbee bee;
  setupBee(bee, ZIGBEE_ROLE_ROUTER, true);

  while (true) {
    bee.process();
    LEDPin::set(bee.isOnline);
    report("Permitting joining", bee.permitJoiningRequest(0xFFFC, 0xFF));
    Tick::delay(10000);
  }

  // while (!bee.isOnline)
  //   bee.process();
  //
  // DELAY(2000000);
  //
  // uint8_t reqData[] = {0x11, 0x22, 0x33};
  //
  // while (true) {
  //   DebugPrint("[Main]  Sending data request...\n");
  //   if (bee.dataRequest(0x0000, 0x01, 0x01, 0x0006, 0x00, 0x08, 0x0F,
  //                       sizeof(reqData), reqData)) {
  //     DebugPrint("[Main]   Data request was successful!\n");
  //   } else {
  //     DebugPrint("[Main]   Data request failed...\n");
  //   }
  //
  //   DELAY(200000);
  // }
  //
  while (true) {
  }
}
