#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include <util/delay.h>

#include "Tick.h"
#include "TinySerial.h"
#include "ZClient.h"
#include "Zigbee.h"

const int PIN_NETWORK_LED = 0;
const int PIN_INPUT = 2;

const int PIN_ZIGBEE_TX = 3;
const int PIN_ZIGBEE_RX = 4;

const uint8_t WDT_SLEEP_FLAG_8S = _BV(WDP3) | _BV(WDP0);
const uint8_t WDT_SLEEP_FLAG_1S = _BV(WDP2) | _BV(WDP1);
const uint8_t WDT_SLEEP_FLAG = WDT_SLEEP_FLAG_8S;
const size_t WDT_SLEEP_MULTIPLIER = 7;

ZClient client(PIN_ZIGBEE_TX, PIN_ZIGBEE_RX, ZClient::SMOKE_DETECTOR);

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

ISR(WDT_vect) {}

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
    PORTB &= ~_BV(PIN_NETWORK_LED);
  } else {
    PORTB |= _BV(PIN_NETWORK_LED);
  }

  if (client.ready) {
    if ((PINB & _BV(PIN_INPUT)) == 0) {
      client.unicast(0x0000, "report", "value", false);
    } else {
      client.unicast(0x0000, "report", "value", true);
    }
  }

  for (size_t i = 0; i < WDT_SLEEP_MULTIPLIER; i++) {
    deepSleep(WDT_SLEEP_FLAG);
  }
}

int main() {
  setup();
  while (1) {
    loop();
  }

  return 0;
}
