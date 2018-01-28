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

const uint8_t WDT_SLEEP_FLAG_16MS = 0b00000000;
const uint8_t WDT_SLEEP_FLAG_32MS = 0b00000001;
const uint8_t WDT_SLEEP_FLAG_64MS = 0b00000010;
const uint8_t WDT_SLEEP_FLAG_128MS = 0b00000011;
const uint8_t WDT_SLEEP_FLAG_256MS = 0b00000100;
const uint8_t WDT_SLEEP_FLAG_512MS = 0b00000101;
const uint8_t WDT_SLEEP_FLAG_1S = 0b00000110;
const uint8_t WDT_SLEEP_FLAG_2S = 0b00000111;
const uint8_t WDT_SLEEP_FLAG_4S = 0b00100000;
const uint8_t WDT_SLEEP_FLAG_8S = 0b00100001;

const size_t NETWORK_LED_BLINK_TIME_MS = 200;

const uint8_t WDT_SLEEP_FLAG = WDT_SLEEP_FLAG_1S;
const size_t ROUTINE_REPORT_INTERVAL = 900;
const size_t MINIMUM_ATTEMPT_GAP = 1;

ZClient client(PIN_ZIGBEE_TX, PIN_ZIGBEE_RX, ZClient::SMOKE_DETECTOR);

void deepSleep(uint8_t timeFlag) {
  // Disables pin change interrupt during deep sleep
  GIMSK &= ~_BV(PCIE);

  wdt_reset();
  MCUSR = 0;
  WDTCR |= (_BV(WDCE) | _BV(WDE));
  WDTCR = _BV(WDIE) | timeFlag;

  sei();
  sleep_enable();
  sleep_cpu();
  sleep_disable();

  GIMSK |= _BV(PCIE);
}

ISR(WDT_vect) {}

void setup() {
  DDRB |= _BV(PIN_NETWORK_LED);

  Tick::begin();

  PORTB |= _BV(PIN_NETWORK_LED);
  client.begin();
  PORTB &= ~_BV(PIN_NETWORK_LED);

  ADCSRA &= ~_BV(ADEN);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void loop() {
  static bool lastReportedValue = false;
  static bool hasSentHello = false;
  static size_t sleepsUntilLastAttempt = MINIMUM_ATTEMPT_GAP;
  static size_t sleepsSinceLastReport = ROUTINE_REPORT_INTERVAL;

  bool value = ((PINB & _BV(PIN_INPUT)) != 0);

  if ((sleepsUntilLastAttempt >= MINIMUM_ATTEMPT_GAP) &&
      ((sleepsSinceLastReport >= ROUTINE_REPORT_INTERVAL) ||
       (value != lastReportedValue))) {
    client.bee.wakeUp();

    if (!client.checkReadyStatus()) {
      PORTB |= _BV(PIN_NETWORK_LED);
      _delay_ms(NETWORK_LED_BLINK_TIME_MS);
      PORTB &= ~_BV(PIN_NETWORK_LED);
    }

    sleepsUntilLastAttempt = 0;

    if (client.ready) {
      lastReportedValue = value;
      sleepsSinceLastReport = 0;

      if (!hasSentHello) {
        hasSentHello = true;
        client.unicast(0x0000, "hello", "", "");
      }

      client.unicast(0x0000, "report", "value", value);
    }
  }

  sleepsSinceLastReport++;
  sleepsUntilLastAttempt++;

  deepSleep(WDT_SLEEP_FLAG);
}

int main() {
  setup();
  while (1) {
    loop();
  }

  return 0;
}
