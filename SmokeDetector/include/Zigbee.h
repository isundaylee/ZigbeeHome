#pragma once

#include "GPIO.h"
#include "RingBuffer.h"
#include "USART.h"
#include "Utils.h"

const uint16_t ZIGBEE_CMD_SYS_RESET_IND = 0x4180;

template <typename USART, typename ResetPin> class Zigbee {
private:
  RingBuffer<uint8_t, 128> command_;

  int commandDataLength() { return command_[1]; }
  int commandTotalLength() { return 1 + 1 + 2 + commandDataLength() + 1; }

public:
  bool isPowered = false;

  Zigbee() {}

  void init() {
    USART::init();

    ResetPin::setMode(GPIO_MODE_OUTPUT);
  }

  uint8_t calculateFCS() {
    uint8_t fcs = 0;
    for (int i = 1; i < commandTotalLength() - 1; i++) {
      fcs ^= command_[i];
    }
    return fcs;
  }

  void processCommand() {
    uint16_t cmd = (command_[2] << 8) | command_[3];

    if (calculateFCS() != command_[command_.size() - 1]) {
      command_.clear();
      return;
    }

    switch (cmd) {
    case ZIGBEE_CMD_SYS_RESET_IND:
      isPowered = true;
      break;
    }

    command_.clear();
  }

  void process() {
    int c;
    while ((c = USART::read()) != -1) {
      uint8_t byte = (uint8_t)c;
      if (command_.empty()) {
        // We're not in the middle of a command
        if (byte != 0xFE) {
          continue;
        }

        command_.push(byte);
      } else {
        // We're in the middle of a command
        command_.push(byte);

        if (command_.size() >= 2 && command_.size() == commandTotalLength()) {
          processCommand();
        }
      }
    }
  }

  void reset() {
    ResetPin::clear();
    DELAY(20000);
    ResetPin::set();

    isPowered = false;
  }
};
