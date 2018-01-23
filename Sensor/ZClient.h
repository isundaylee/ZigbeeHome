#pragma once

#include "Zigbee.h"

class ZClient {
private:
  static const size_t MAX_SIZE = 256;

  static const uint8_t BEGIN = 0x00;
  static const uint8_t END = 0xFF;
  static const uint8_t ESC = 0xFE;
  static const uint8_t ESC_BEGIN = 0x01;
  static const uint8_t ESC_END = 0x02;
  static const uint8_t ESC_ESC = 0x03;

  uint8_t *encodeByte(uint8_t byte, uint8_t *ptr);
  uint8_t *encodeBytes(uint8_t *buf, size_t len, uint8_t *ptr);
  uint8_t *encodeString(const char *str, uint8_t *ptr);

public:
  Zigbee bee;
  uint8_t macAddress[8];

  bool ready = false;

  ZClient(int txPin, int rxPin);

  void begin();

  bool checkReadyStatus();
  void waitUntilReady();
  bool broadcast(const char *method, const char *key, const char *value);
};
