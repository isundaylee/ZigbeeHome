#pragma once

#include "TinySerial.h"

class Zigbee {
private:
  static const int INPUT_FLUSH_PREDELAY;
  static const int WAKEUP_DELAY;
  static const int QUERY_DELAY;

  TinySerial serial_;

  bool query(const char *query, uint8_t *out, size_t responseSize);
  bool waitForBytes(size_t count, size_t maxWaitTime);
  void flushSerialInput();
  void wakeUp();

public:
  Zigbee(int txPin, int rxPin);

  void begin(long baud);

  void broadcast(uint8_t *buf, size_t len);
  void broadcast(const char *buf);

  void getMacAddress(uint8_t *out);
};
