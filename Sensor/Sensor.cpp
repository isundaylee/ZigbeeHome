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

ZClient client(PIN_ZIGBEE_TX, PIN_ZIGBEE_RX);

ISR(WDT_vect) {}

void setup() {
  DDRB |= 4;

  client.begin();

  ADCSRA &= ~_BV(ADEN);
}

void loop() {
  client.broadcast("ping", "type", "fake_light_bulb");

  GIMSK &= ~_BV(PCIE);

  wdt_reset();
  MCUSR = 0;
  WDTCR |= (_BV(WDCE) | _BV(WDE));
  WDTCR = (_BV(WDIE) | 0b100001);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sei();
  sleep_mode();

  GIMSK |= _BV(PCIE);
}

int main() {
  setup();
  while (1) {
    loop();
  }

  return 0;
}
