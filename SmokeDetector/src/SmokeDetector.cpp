#include <stm32l011xx.h>

#include "Clock.h"
#include "GPIO.h"
#include "Tick.h"
#include "USART.h"
// #include "Zigbee.h"
#include "ADC.h"
#include "Utils.h"

typedef GPIO_A::Pin<4> LEDPin;
// typedef Zigbee<USART_2, GPIO_A::Pin<7>> MyZigbee;

// uint8_t report(const char *action, uint8_t result) {
//   DebugPrint("[Main]   ");
//   DebugPrint(action);
//   if (result == ZIGBEE_STATUS_SUCCESS) {
//     DebugPrint(" succeeded! \n");
//   } else {
//     DebugPrint(" failed with error ");
//     DebugPrintHex(result);
//     DebugPrint("... \n");
//   }
//
//   return result;
// }

// void setupBee(MyZigbee &bee, uint8_t role, bool reset = false) {
//   bee.init();
//
//   report("Reset", bee.reset());
//
//   if (reset) {
//     report("Reset", bee.resetSettings());
//     report("Setting role", bee.setRole(role));
//     report("Setting primary channel", bee.setChannelMask(true, 0x00002000));
//     report("Setting secondary channel", bee.setChannelMask(false,
//     0x00002000));
//   }
//
//   uint16_t clusters[] = {0x0000, 0x0006};
//   report("Registration", bee.registerEndpoint(0x01, 0x0104, 0x0100, 0x00, 2,
//                                               clusters, 2, clusters));
//   report("Starting network", bee.startup());
// }

void notmain(void) {
  Tick::init();

  DebugUART::init();
  DebugPrint("[Main]   Hello, world!\n");

  LEDPin::GPIO::init();
  LEDPin::setMode(GPIO_MODE_OUTPUT);
  LEDPin::clear();

  ADC_1::init();
  DebugPrint("[Main]   ADC initialized!\n");

  // ADC_1::enableTemperatureSensor();
  ADC_1::selectChannel(0);
  while (true) {
    DebugPrintHex(ADC_1::convert());
    DebugPrint("\n");
  }

  // MyZigbee bee;
  //
  // uint8_t reqData[] = {0x11, 0x22, 0x33};
  //
  // while (true) {
  //   uint8_t errors = 0;
  //
  //   setupBee(bee, ZIGBEE_ROLE_ROUTER, false);
  //   uint32_t start = Tick::value;
  //   while (!Tick::hasElapsedSince(start, 10000)) {
  //     bee.process();
  //     if (bee.isOnline) {
  //       LEDPin::set();
  //       break;
  //     }
  //   }
  //   if (!bee.isOnline) {
  //     continue;
  //   }
  //
  //   while (true) {
  //     if (report("Sending data",
  //                bee.dataRequest(0x0000, 0x01, 0x01, 0x0006, 0x00, 0x08,
  //                0x0F,
  //                                sizeof(reqData), reqData)) !=
  //         ZIGBEE_STATUS_SUCCESS) {
  //       errors++;
  //
  //       if (errors >= 3) {
  //         LEDPin::clear();
  //         break;
  //       }
  //     }
  //     Tick::delay(1000);
  //   }
  // }
}
