#include <Arduino.h>
#include <SoftwareSerial.h>

#include "Zigbee.h"

const int PIN_ZIGBEE_TX = 3;
const int PIN_ZIGBEE_RX = 4;

const int PIN_LED = 0;

Zigbee zigbee(PIN_ZIGBEE_TX, PIN_ZIGBEE_RX);

void setup() { zigbee.begin(9600); }

void loop() {
  static uint8_t c = 0;

  zigbee.broadcast(&c, 1);
  c++;

  delay(300);
}
