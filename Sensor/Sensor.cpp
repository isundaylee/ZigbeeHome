#include <Arduino.h>
#include <SoftwareSerial.h>
#include <AsmTinySerial.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "ZClient.h"

const int PIN_ZIGBEE_TX = 3;
const int PIN_ZIGBEE_RX = 4;

const int PIN_INPUT = 1;
const int PIN_LED = 0;

ZClient client(PIN_ZIGBEE_TX, PIN_ZIGBEE_RX);

void deep_sleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  sleep_enable();
  sleep_mode();
  sleep_disable();
}

// Taken from: https://gist.github.com/dwhacks/8055287
void setup_watchdog(int ii) {
  byte bb;
  if (ii > 9)
    ii = 9;
  bb = ii & 7;
  if (ii > 7)
    bb |= (1 << 5);
  bb |= (1 << WDCE);

  MCUSR &= ~(1 << WDRF);
  // start timed sequence
  WDTCR |= (1 << WDCE) | (1 << WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}

void setup() {
  SerialInit(PB3, 115200);

  uint8_t buf[2];
  buf[1] = 0;

  while (true) {
    SerialTx("\xFC\x22\x01\x01");
    for (uint8_t j=0; j<32; j++) {
      SerialTx(buf);
    }
    buf[0]++;
    delay(500);
  }

  digitalWrite(PIN_LED, HIGH);
  // client.begin(28800);
  digitalWrite(PIN_LED, LOW);

  // Disable ADC
  ADCSRA &= ~(1 << ADEN);

  pinMode(PIN_INPUT, INPUT);
  pinMode(PIN_LED, OUTPUT);
}

void loop() {
  digitalWrite(PIN_LED, HIGH);
  if (digitalRead(PIN_INPUT)) {
    client.broadcast("report", "value", "1");
  } else {
    client.broadcast("report", "value", "0");
  }
  digitalWrite(PIN_LED, LOW);

  delay(2000);

  // setup_watchdog(9);
  // deep_sleep();
}

ISR(WDT_vect) {}
