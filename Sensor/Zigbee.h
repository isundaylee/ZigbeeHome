#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>

class Zigbee {
private:
  static const int WAKEUP_DELAY;

  SoftwareSerial serial_;

  void flushSerialInput();
  void wakeUp();

public:
  Zigbee(int txPin, int rxPin);

  void begin(int baud);

  void broadcast(uint8_t *buf, size_t len);
  void broadcast(uint8_t *buf);
};
