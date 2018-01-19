#include <Arduino.h>
#include <SoftwareSerial.h>

#include "ZClient.h"

const int PIN_ZIGBEE_TX = 3;
const int PIN_ZIGBEE_RX = 4;

const int PIN_INPUT = 1;
const int PIN_LED = 0;

ZClient client(PIN_ZIGBEE_TX, PIN_ZIGBEE_RX);

void setup() {
  client.begin(9600);

  pinMode(PIN_INPUT, INPUT);
}

void loop() {
  client.broadcast("ping", "type", "generic_sensor");

  delay(2000);

  if (digitalRead(PIN_INPUT)) {
    client.broadcast("report", "value", "1");
  } else {
    client.broadcast("report", "value", "0");
  }

  delay(9000);
}
