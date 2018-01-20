#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/delay.h>

#include "TinySerial.h"

const int PIN_ZIGBEE_TX = 3;
const int PIN_ZIGBEE_RX = 4;

const int PIN_INPUT = 1;
const int PIN_LED = 0;

TinySerial serial(PIN_ZIGBEE_TX, PIN_ZIGBEE_RX);

void setup() { serial.begin(115200); }

void loop() {
  while (serial.available()) {
    serial.write(serial.read());
  }

  _delay_ms(1);
}

int main() {
  setup();
  while (1) {
    loop();
  }

  return 0;
}
