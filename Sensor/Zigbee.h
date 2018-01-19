#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>

class Zigbee {
private:
  static const int INPUT_FLUSH_PREDELAY;
  static const int WAKEUP_DELAY;

  SoftwareSerial serial_;

  void query(char *query, uint8_t *out, size_t responseSize);
  void waitForBytes(size_t count);
  void flushSerialInput();
  void wakeUp();

public:
  Zigbee(int txPin, int rxPin);

  void begin(int baud);

  void broadcast(uint8_t *buf, size_t len);
  void broadcast(char *buf);

  void getMacAddress(uint8_t *out);
};
