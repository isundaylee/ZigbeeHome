#pragma once

#include "TinySerial.h"
#include "Tick.h"

typedef uint16_t addr_t;

class Zigbee {
private:
  static const int WAKEUP_MESSAGE_LENGTH;
  static const int WAKEUP_WAIT_TIME_INITIAL;
  static const int WAKEKE_WAIT_TIME_BACKOFF_FACTOR;
  static const int WAKEUP_WAIT_TIME_MAX;
  static const int WAKEUP_MAX_TRIES;
  static const int QUERY_QUESCE_TIME;
  static const int QUERY_WAIT_TIME;
  static const int MESSAGE_GAP_DELAY;

  TinySerial serial_;
  tick_t lastOutTime_;

  void beforeMessage();
  void afterMessage();
  bool query(const char *query, uint8_t *out, size_t responseSize);
  bool waitForBytes(size_t count, size_t maxWaitTime);

public:
  static const int NETWORK_DOWN = 0;
  static const int NETWORK_UP = 1;

  static const int ERROR = -1;

  Zigbee(int txPin, int rxPin);

  void begin();

  bool wakeUp();

  bool broadcast(uint8_t *buf, size_t len);
  bool broadcast(const char *buf);

  bool unicast(addr_t addr, uint8_t *buf, size_t len);
  bool unicast(addr_t addr, const char *buf);

  bool getMacAddress(uint8_t *out);
  int getNetworkState();
};
