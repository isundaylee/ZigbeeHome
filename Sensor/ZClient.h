#pragma once

#include "Zigbee.h"

typedef uint8_t device_t;
typedef uint8_t value_type_t;

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

  uint8_t *encodeValue(bool value, uint8_t *ptr);
  uint8_t *encodeValue(const char *str, uint8_t *ptr);
  uint8_t *encodeValue(uint8_t byte, uint8_t *ptr);

public:
  static const device_t SMOKE_DETECTOR = 0x01;

  static const value_type_t BOOL = 0x01;
  static const value_type_t STRING = 0x02;
  static const value_type_t UINT8 = 0x03;

  Zigbee bee;
  uint8_t macAddress[8];
  device_t deviceType;

  bool ready = false;

  ZClient(int txPin, int rxPin, device_t deviceType);

  void begin();

  bool checkReadyStatus();
  void waitUntilReady();

  template <typename ValueType>
  bool unicast(addr_t addr, const char *method, const char *key,
               ValueType value) {
    if (!ready) {
      return false;
    }

    static uint8_t buf[MAX_SIZE];
    uint8_t *ptr = buf;

    *(ptr++) = BEGIN;
    ptr = encodeBytes(macAddress, 8, ptr);
    ptr = encodeByte(deviceType, ptr);
    ptr = encodeValue(method, ptr);
    ptr = encodeValue(key, ptr);
    ptr = encodeValue(value, ptr);
    *(ptr++) = END;

    if (!bee.unicast(addr, buf, ptr - buf)) {
      ready = false;
      return false;
    } else {
      return true;
    }
  }
};
