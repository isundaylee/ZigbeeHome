#pragma once

#include "TinySerial.h"

class Zigbee {
private:
  static const int WAKEUP_MESSAGE_LENGTH;
  static const int WAKEUP_WAIT_TIME;
  static const int QUERY_WAIT_TIME;
  static const int MESSAGE_GAP_DELAY;

  TinySerial serial_;

  bool query(const char *query, uint8_t *out, size_t responseSize);
  bool waitForBytes(size_t count, size_t maxWaitTime);
  void flushSerialInput();
  void wakeUp();

public:
  Zigbee(int txPin, int rxPin);

  void begin();

  void broadcast(uint8_t *buf, size_t len);
  void broadcast(const char *buf);

  void getMacAddress(uint8_t *out);
};
