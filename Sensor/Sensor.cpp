#include <Arduino.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "TinySerial.h"

const int PIN_ZIGBEE_TX = 3;
const int PIN_ZIGBEE_RX = 4;

const int PIN_INPUT = 1;
const int PIN_LED = 0;

TinySerial serial(PIN_ZIGBEE_TX, PIN_ZIGBEE_RX);

void setup() {
  serial.begin(115200);
}

void loop() {
  static uint8_t i = 0;

  serial.write(0xFC);
  serial.write(0x42);
  serial.write(0x01);
  serial.write(0x01);

  for (size_t j = 0; j < 64; j++) {
    serial.write(i);
  }

  i++;

  delay(500);
}
