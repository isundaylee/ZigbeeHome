#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include <util/delay.h>

#include "Tick.h"
#include "TinySerial.h"
#include "ZClient.h"
#include "Zigbee.h"

const int PIN_NETWORK_LED = 0;
const int PIN_INPUT = 1;

const int PIN_ZIGBEE_TX = 3;
const int PIN_ZIGBEE_RX = 4;

const uint8_t WDT_SLEEP_FLAG = _BV(WDP3) | _BV(WDP0);
const size_t WDT_SLEEP_MULTIPLIER = 2;

ZClient client(PIN_ZIGBEE_TX, PIN_ZIGBEE_RX);

void deepSleep(uint8_t timeFlag) {
  // Disables pin change interrupt  and pin output during deep sleep
  GIMSK &= ~_BV(PCIE);

  wdt_reset();
  MCUSR = 0;
  WDTCR |= (_BV(WDCE) | _BV(WDE));
  WDTCR = _BV(WDIE) | timeFlag;

  sei();
  sleep_enable();
  sleep_cpu();
  sleep_disable();
  // sleep_mode();

  GIMSK |= _BV(PCIE);
}

ISR(WDT_vect) {
  static int counter = 0;

  counter++;

  if (counter == WDT_SLEEP_MULTIPLIER) {
    counter = 0;
  } else {
    deepSleep(WDT_SLEEP_FLAG);
  }
}

void setup() {
  DDRB |= _BV(PIN_NETWORK_LED);

  Tick::begin();
  client.begin();

  ADCSRA &= ~_BV(ADEN);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void loop() {
  client.bee.wakeUp();

  if (client.checkReadyStatus()) {
    PORTB |= _BV(PIN_NETWORK_LED);
  } else {
    PORTB &= ~_BV(PIN_NETWORK_LED);
  }

  if (client.ready) {
    client.broadcast("ping", "type", "fake_light_bulb");
  }

  deepSleep(WDT_SLEEP_FLAG);
}

int main() {
  setup();
  while (1) {
    loop();
  }

  return 0;
}
