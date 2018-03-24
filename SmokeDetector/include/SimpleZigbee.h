#pragma once

#include "Utils.h"
#include "Zigbee.h"

const int TIMEOUT_FAILURES_THRESHOLD = 5;
const int SEND_FAILURES_THRESHOLD = 3;

template <typename USART, typename ResetPin, typename WakeUpPin>
class SimpleZigbee {
private:
  uint8_t role_;
  uint16_t panId_;
  int sendFailures_ = 0;
  int timeoutFailures_ = 0;

  bool report(const char *action, uint8_t result) {
    DebugPrint("[Bee]    ");
    DebugPrint(action);
    if (result == ZIGBEE_STATUS_SUCCESS) {
      DebugPrint(" succeeded! \n");
      return true;
    } else {
      if (result == ZIGBEE_STATUS_TIMEOUT) {
        timeoutFailures_++;
      } else {
        timeoutFailures_ = 0;
      }

      DebugPrint(" failed with error ");
      DebugPrintHex(result);
      DebugPrint("... \n");
      return false;
    }
  }

  bool setup(bool reconfigure) {
    sendFailures_ = 0;
    timeoutFailures_ = 0;

    // First we always power cycle the Zigbee chip.
    if (!report("Reset", bee.reset()))
      return false;

    if (reconfigure) {
      // Here we reconfigure the Zigbee chip if necessary
      if (!report("Reset", bee.resetSettings()))
        return false;

      if (!report("Setting role", bee.setRole(role_)))
        return false;

      if (!report("Setting PAN ID", bee.setPANId(panId_)))
        return false;

      if (!report("Setting primary channel",
                  bee.setChannelMask(true, 0x00002000)))
        return false;

      if (!report("Setting secondary channel",
                  bee.setChannelMask(false, 0x00002000)))
        return false;
    }

    if (!report("Setting TX power", bee.setTxPower(20)))
      return false;

    uint16_t clusters[] = {0x0000, 0x0006};
    if (!report("Registration", bee.registerEndpoint(0x01, 0x0104, 0x0100, 0x00,
                                                     2, clusters, 2, clusters)))
      return false;

    if (role_ == ZIGBEE_ROLE_COORDINATOR) {
      if (!report("Network formation",
                  bee.startCommissioning(
                      ZIGBEE_COMMISSIONING_MODE_NETWORK_FORMATION)))
        return false;
    }

    if (!report(
            "Network steering",
            bee.startCommissioning(ZIGBEE_COMMISSIONING_MODE_NETWORK_STEERING)))
      return false;
  }

public:
  Zigbee<USART, ResetPin, WakeUpPin> bee;

  SimpleZigbee(uint8_t role, uint16_t panId) : role_(role), panId_(panId) {}

  void init() { bee.init(); }

  bool isConnected() {
    auto state = bee.zdoState;

    if (timeoutFailures_ >= TIMEOUT_FAILURES_THRESHOLD) {
      DebugPrint(
          "[Bee]    Considered not connected due to timeout failures.\n");
      return false;
    }

    if (sendFailures_ >= SEND_FAILURES_THRESHOLD) {
      DebugPrint("[Bee]    Considered not connected due to send failures.\n");
      return false;
    }

    return (state == ZIGBEE_ZDO_STATE_ZB_COORD ||
            state == ZIGBEE_ZDO_STATE_ROUTER ||
            state == ZIGBEE_ZDO_STATE_END_DEVICE);
  }

  bool isPowered() { return bee.isPowered; }

  bool connect(bool reconfigure, uint32_t timeout) {
    uint32_t start = Tick::value;

    bee.wakeUp();
    setup(reconfigure);

    while (!Tick::hasElapsedSince(start, timeout)) {
      bee.process();

      if (isConnected()) {
        bee.sleep();
        return true;
      }
    }

    bee.sleep();
    return false;
  }

  bool send(uint16_t dst, uint8_t dataLen, uint8_t *data) {
    static uint8_t transId = 0;

    bee.wakeUp();
    uint8_t result = bee.dataRequest(dst, 0x01, 0x01, 0x0006, transId++, 0x00,
                                     0x0F, dataLen, data);
    bee.sleep();

    bool success = report("Sending data", result);

    if (success) {
      sendFailures_ = 0;
    } else {
      sendFailures_++;
    }

    return success;
  }
};
