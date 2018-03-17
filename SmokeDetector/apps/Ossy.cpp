#include <stm32l011xx.h>

#include "ADC.h"
#include "Clock.h"
#include "Tick.h"
#include "Utils.h"

const static int numSamples = 512;
const static int interval = 2;
static uint16_t samples[numSamples];

extern "C" void main(void) {
  Clock::enableHSI();
  Clock::selectSystemClock(RCC_CFGR_SW_HSI);
  Tick::init();

  ADC_1::init();
  ADC_1::enableVoltageReference();

  DebugUART::init();
  DebugPrint("Taking ");
  DebugPrintDec(numSamples);
  DebugPrint(" samples ");
  DebugPrintDec(interval);
  DebugPrint(" ms apart.\n");

  ADC_1::selectChannel(ADC_CHANNEL_VOLTAGE_REFERENCE);
  uint32_t vref = ADC_1::convert();

  ADC_1::selectChannel(7);

  for (int i = 0; i < numSamples; i++) {
    uint32_t start = Tick::value;

    uint32_t reading = ADC_1::convert();
    samples[i] =
        (3 * ADC_1::getVoltageReferenceCalibrationValue() * reading) / vref;

    Tick::waitUntil(start + interval);
  }

  DebugPrint("{");
  for (int i = 0; i < numSamples; i++) {
    if (i != 0) {
      DebugPrint(", ");
    }

    DebugPrintDec(samples[i]);
  }
  DebugPrint("}\n");
}
