#pragma once

#include <stm32l0xx.h>

#include "Utils.h"

const uint8_t ADC_CHANNEL_VOLTAGE_REFERENCE = 17;
const uint8_t ADC_CHANNEL_TEMPERATURE_SENSOR = 18;

const uint32_t ADC_VOLTAGE_REFERENCE_CALIBRATION_ADDR = 0x1FF80078;

class ADC_1 {
public:
  static void init() {
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE_Msk;
    ADC1->CFGR2 |= (0b10 << ADC_CFGR2_CKMODE_Pos);

    ADC1->CR &= ~ADC_CR_ADEN;

    ADC1->CR |= ADC_CR_ADCAL;
    WAIT_UNTIL((ADC1->ISR & ADC_ISR_EOCAL) != 0);
    ADC1->ISR |= ADC_ISR_EOCAL;

    ADC1->CR |= ADC_CR_ADEN;
    WAIT_UNTIL((ADC1->ISR & ADC_ISR_ADRDY) != 0);
  }

  static void enableVoltageReference() { ADC->CCR |= ADC_CCR_VREFEN; }
  static void enableTemperatureSensor() { ADC->CCR |= ADC_CCR_TSEN; }

  static void selectChannel(uint8_t ch) { ADC1->CHSELR = (1 << ch); }

  static uint16_t convert() {
    ADC1->CR |= ADC_CR_ADSTART;
    WAIT_UNTIL((ADC1->ISR & ADC_ISR_EOSEQ) != 0);
    return ADC1->DR;
  }

  static uint16_t getVoltageReferenceCalibrationValue() {
    return *reinterpret_cast<uint16_t *>(
        ADC_VOLTAGE_REFERENCE_CALIBRATION_ADDR);
  }
};
