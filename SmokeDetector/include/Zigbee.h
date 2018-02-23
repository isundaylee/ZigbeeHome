#pragma once

#include "GPIO.h"
#include "RingBuffer.h"
#include "USART.h"
#include "Utils.h"

const uint16_t ZIGBEE_CMD_APP_CNF_BDB_COMMISSIONING_NOTIFICATION = 0x4F80;
const uint16_t ZIGBEE_CMD_APP_CNF_BDB_SET_CHANNEL = 0x2F08;
const uint16_t ZIGBEE_CMD_APP_CNF_BDB_START_COMMISSIONING = 0x2F05;
const uint16_t ZIGBEE_CMD_SYS_OSAL_NV_WRITE = 0x2109;
const uint16_t ZIGBEE_CMD_SYS_RESET_IND = 0x4180;
const uint16_t ZIGBEE_CMD_ZDO_MGMT_PERMIT_JOIN_RSP = 0x45B6;
const uint16_t ZIGBEE_CMD_ZDO_NODE_DESC_RSP = 0x4582;
const uint16_t ZIGBEE_CMD_ZDO_STATE_CHANGE_IND = 0x45C0;

const uint16_t ZIGBEE_CMD_SYNC_REPLY_MASK = 0x4000;
const uint16_t ZIGBEE_CMD_INVALID = 0xFFFF;

const uint8_t ZIGBEE_ROLE_COORDINATOR = 0x00;
const uint8_t ZIGBEE_ROLE_ROUTER = 0x01;
const uint8_t ZIGBEE_ROLE_END_DEVICE = 0x02;

const uint8_t ZIGBEE_COMMISSIONING_MODE_INITIALIZATION = 0x00;
const uint8_t ZIGBEE_COMMISSIONING_MODE_TOUCH_LINK = 0x01;
const uint8_t ZIGBEE_COMMISSIONING_MODE_NETWORK_STEERING = 0x02;
const uint8_t ZIGBEE_COMMISSIONING_MODE_NETWORK_FORMATION = 0x04;
const uint8_t ZIGBEE_COMMISSIONING_MODE_NETWORK_FINDING_AND_BINDING = 0x08;

const uint8_t ZIGBEE_ZDO_STATE_HOLD = 0x00;
const uint8_t ZIGBEE_ZDO_STATE_INIT = 0x01;
const uint8_t ZIGBEE_ZDO_STATE_NWK_DISC = 0x02;
const uint8_t ZIGBEE_ZDO_STATE_NWK_JOINING = 0x03;
const uint8_t ZIGBEE_ZDO_STATE_REJOINING = 0x04;
const uint8_t ZIGBEE_ZDO_STATE_END_DEVICE_UNAUTH = 0x05;
const uint8_t ZIGBEE_ZDO_STATE_END_DEVICE = 0x06;
const uint8_t ZIGBEE_ZDO_STATE_ROUTER = 0x07;
const uint8_t ZIGBEE_ZDO_STATE_COORD_STARTING = 0x08;
const uint8_t ZIGBEE_ZDO_STATE_ZB_COORD = 0x09;
const uint8_t ZIGBEE_ZDO_STATE_NWK_ORPHAN = 0x0A;

const uint16_t ZIGBEE_SYNC_COMMANDS[] = {
    ZIGBEE_CMD_SYS_OSAL_NV_WRITE, ZIGBEE_CMD_APP_CNF_BDB_SET_CHANNEL,
    ZIGBEE_CMD_APP_CNF_BDB_START_COMMISSIONING};

template <typename USART, typename ResetPin> class Zigbee {
private:
  RingBuffer<uint8_t, 128> command_;

  uint16_t pendingSyncCommand_;
  uint8_t lastSyncCommandStatus_;

  int commandDataLength() { return command_[1]; }
  int commandTotalLength() { return 1 + 1 + 2 + commandDataLength() + 1; }

public:
  bool isPowered = false;
  uint8_t zdoState = 0;

  Zigbee() {}

  void init() {
    USART::init();

    ResetPin::GPIO::init();
    ResetPin::setMode(GPIO_MODE_OUTPUT);
  }

  uint8_t calculateFCS() {
    uint8_t fcs = 0;
    for (int i = 1; i < commandTotalLength() - 1; i++) {
      fcs ^= command_[i];
    }
    return fcs;
  }

  char *humanReadableZDOState(uint8_t state) {
    switch (state) {
    case ZIGBEE_ZDO_STATE_HOLD:
      return "HOLD";
    case ZIGBEE_ZDO_STATE_INIT:
      return "INIT";
    case ZIGBEE_ZDO_STATE_NWK_DISC:
      return "NWK_DISC";
    case ZIGBEE_ZDO_STATE_NWK_JOINING:
      return "NWK_JOINING";
    case ZIGBEE_ZDO_STATE_REJOINING:
      return "REJOINING";
    case ZIGBEE_ZDO_STATE_END_DEVICE_UNAUTH:
      return "END_DEVICE_UNAUTH";
    case ZIGBEE_ZDO_STATE_END_DEVICE:
      return "END_DEVICE";
    case ZIGBEE_ZDO_STATE_ROUTER:
      return "ROUTER";
    case ZIGBEE_ZDO_STATE_COORD_STARTING:
      return "COORD_STARTING";
    case ZIGBEE_ZDO_STATE_ZB_COORD:
      return "ZB_COORD";
    case ZIGBEE_ZDO_STATE_NWK_ORPHAN:
      return "NWK_ORPHAN";
    default:
      return "!!!UNKNOWN!!!";
    }
  }

  void processCommand() {
    // command_[1]: Payload length.
    // command_[2-3]: Command.
    // command_[4-]: Payload.

    if (calculateFCS() != command_[command_.size() - 1]) {
      command_.clear();
      return;
    }

    uint16_t cmd = (command_[2] << 8) | command_[3];

    for (int i = 0;
         i < sizeof(ZIGBEE_SYNC_COMMANDS) / sizeof(ZIGBEE_SYNC_COMMANDS[0]);
         i++) {
      uint16_t syncCommand = ZIGBEE_SYNC_COMMANDS[i];
      if (cmd == (syncCommand | ZIGBEE_CMD_SYNC_REPLY_MASK)) {
        if (pendingSyncCommand_ == syncCommand) {
          pendingSyncCommand_ = ZIGBEE_CMD_INVALID;
          lastSyncCommandStatus_ = command_[4];
        }
        command_.clear();
        return;
      }
    }

    switch (cmd) {
    case ZIGBEE_CMD_SYS_RESET_IND:
      isPowered = true;
      break;
    case ZIGBEE_CMD_ZDO_STATE_CHANGE_IND:
      zdoState = command_[4];
      DebugPrint("[Zigbee] ZDO state changed to ");
      DebugPrint(humanReadableZDOState(zdoState));
      DebugPrint(".\n");
      break;
    case ZIGBEE_CMD_ZDO_NODE_DESC_RSP:
    case ZIGBEE_CMD_ZDO_MGMT_PERMIT_JOIN_RSP:
    case ZIGBEE_CMD_APP_CNF_BDB_COMMISSIONING_NOTIFICATION:
      // Things we don't care about
      break;
    default:
      DebugPrint("[Zigbee] !!!! Unrecognized command ");
      DebugPrintHex(cmd);
      DebugPrint(". \n");
      break;
    }

    command_.clear();
  }

  void process() {
    int c;
    while ((c = USART::read()) != -1) {
      uint8_t byte = static_cast<uint8_t>(c);

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

  bool sendSyncCommand(uint16_t command, uint8_t dataSize, uint8_t *data) {
    DELAY(20000);

    pendingSyncCommand_ = command;

    USART::write(static_cast<uint8_t>(0xFE));
    USART::write(static_cast<uint8_t>(dataSize));
    USART::write(static_cast<uint8_t>((command & 0xFF00) >> 8));
    USART::write(static_cast<uint8_t>((command & 0x00FF)));

    uint8_t FCS = dataSize ^ ((command & 0xFF00) >> 8) ^ (command & 0x00FF);

    for (uint8_t i = 0; i < dataSize; i++) {
      FCS ^= data[i];
      USART::write(data[i]);
    }

    USART::write(FCS);

    while (pendingSyncCommand_ != ZIGBEE_CMD_INVALID) {
      process();
    }

    return (lastSyncCommandStatus_ == 0);
  }

  bool resetSettings() {
    uint8_t data[] = {0x03, 0x00, 0x00, 0x01, 0x03};
    if (!sendSyncCommand(ZIGBEE_CMD_SYS_OSAL_NV_WRITE, sizeof(data), data)) {
      return false;
    }

    reset();
    return true;
  }

  bool setRole(uint8_t role) {
    uint8_t data[] = {0x87, 0x00, 0x00, 0x01, role};
    return sendSyncCommand(ZIGBEE_CMD_SYS_OSAL_NV_WRITE, sizeof(data), data);
  }

  bool setChannelMask(bool primary, uint32_t channelMask) {
    uint8_t data[] = {primary ? 0x01 : 0x00, (channelMask & 0x000000FF) >> 0,
                      (channelMask & 0x0000FF00) >> 8,
                      (channelMask & 0x00FF0000) >> 16,
                      (channelMask & 0xFF000000) >> 24};
    return sendSyncCommand(ZIGBEE_CMD_APP_CNF_BDB_SET_CHANNEL, sizeof(data),
                           data);
  }

  bool startCommissioning(uint8_t commissioningMode) {
    uint8_t data[] = {commissioningMode};
    return sendSyncCommand(ZIGBEE_CMD_APP_CNF_BDB_START_COMMISSIONING,
                           sizeof(data), data);
  }

  void reset() {
    ResetPin::clear();
    DELAY(20000);
    ResetPin::set();

    isPowered = false;

    while (!isPowered) {
      process();
    }
  }
};
