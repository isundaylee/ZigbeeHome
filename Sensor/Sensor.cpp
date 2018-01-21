#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include <util/delay.h>

#include "TinySerial.h"
#include "ZClient.h"
#include "Zigbee.h"

const int PIN_ZIGBEE_TX = 3;
const int PIN_ZIGBEE_RX = 4;

const int PIN_INPUT = 1;
const int PIN_LED = 0;

// TinySerial serial(PIN_ZIGBEE_TX, PIN_ZIGBEE_RX);
ZClient client(PIN_ZIGBEE_TX, PIN_ZIGBEE_RX);

ISR(WDT_vect) {}

void setup() {
  client.begin();

  DDRB ^= 4;
}

void loop() {
  client.broadcast("ping", "type", "fake_light_bulb");

  _delay_ms(1000);

  // wdt_reset();
  // MCUSR = 0;
  // WDTCR |= (_BV(WDCE) | _BV(WDE));
  // WDTCR = (_BV(WDIE) | 0b000001);
  //
  // ADCSRA &= ~_BV(ADEN);
  //
  // set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  // sei();
  // sleep_mode();
}

int main() {
  setup();
  while (1) {
    loop();
  }

  return 0;
}
