#include "ZClient.h"

#include <avr/io.h>

#include <util/delay.h>

#include <string.h>

ZClient::ZClient(int txPin, int rxPin) : bee(txPin, rxPin) {}

bool ZClient::checkReadyStatus() {
  ready = (bee.getNetworkState() == Zigbee::NETWORK_UP);

  return ready;
}

void ZClient::waitUntilReady() {
  while (!this->checkReadyStatus()) {
    _delay_ms(100);
  }

  ready = true;
}

void ZClient::begin() {
  bee.begin();

  this->waitUntilReady();

  if (!bee.getMacAddress(macAddress)) {
    ready = false;
  }
}

uint8_t *ZClient::encodeByte(uint8_t byte, uint8_t *ptr) {
  if (byte == BEGIN) {
    *(ptr++) = ESC;
    *(ptr++) = ESC_BEGIN;
  } else if (byte == END) {
    *(ptr++) = ESC;
    *(ptr++) = ESC_END;
  } else if (byte == ESC) {
    *(ptr++) = ESC;
    *(ptr++) = ESC_ESC;
  } else {
    *(ptr++) = byte;
  }

  return ptr;
}

uint8_t *ZClient::encodeBytes(uint8_t *buf, size_t len, uint8_t *ptr) {
  ptr = this->encodeByte(len, ptr);

  for (size_t i = 0; i < len; i++) {
    ptr = this->encodeByte(buf[i], ptr);
  }

  return ptr;
}

uint8_t *ZClient::encodeString(const char *str, uint8_t *ptr) {
  return this->encodeBytes((uint8_t *)str, strlen(str), ptr);
}

bool ZClient::broadcast(const char *method, const char *key,
                        const char *value) {
  if (!ready) {
    return false;
  }

  static uint8_t buf[MAX_SIZE];
  uint8_t *ptr = buf;

  *(ptr++) = BEGIN;
  ptr = encodeBytes(macAddress, 8, ptr);
  ptr = encodeString(method, ptr);
  ptr = encodeString(key, ptr);
  ptr = encodeString(value, ptr);
  *(ptr++) = END;

  if (!bee.broadcast(buf, ptr - buf)) {
    ready = false;
    return false;
  } else {
    return true;
  }
}
