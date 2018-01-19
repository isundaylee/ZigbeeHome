#include <Arduino.h>
#include <SoftwareSerial.h>

#include "Zigbee.h"

const int PIN_ZIGBEE_TX = 3;
const int PIN_ZIGBEE_RX = 4;

const int PIN_LED = 0;

Zigbee zigbee(PIN_ZIGBEE_TX, PIN_ZIGBEE_RX);

void setup() { zigbee.begin(9600); }

void loop() {
  uint8_t addr[8];

  zigbee.getMacAddress(addr);
  zigbee.broadcast(addr, 8);

  delay(1000);
}
