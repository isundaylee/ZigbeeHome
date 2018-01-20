#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "TinySerial.h"
#include "Zigbee.h"
#include "ZClient.h"

const int PIN_ZIGBEE_TX = 3;
const int PIN_ZIGBEE_RX = 4;

const int PIN_INPUT = 1;
const int PIN_LED = 0;

ZClient client(PIN_ZIGBEE_TX, PIN_ZIGBEE_RX);
// Zigbee bee(PIN_ZIGBEE_TX, PIN_ZIGBEE_RX);
// TinySerial serial(PIN_ZIGBEE_TX, PIN_ZIGBEE_RX);

void setup() { client.begin(115200); }

void loop() {
  // serial.write("\xFC\x03\x01\x01\x01");
  // bee.broadcast("\x07");
  client.broadcast("ping", "type", "fake_light_bulb");

  _delay_ms(10000);
}

int main() {
  setup();
  while (1) {
    loop();
  }

  return 0;
}
