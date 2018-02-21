#pragma once

class Interrupt {
public:
  static void enable() { asm("CPSIE i"); }
  static void enableIRQ(IRQn_Type irq) { NVIC_EnableIRQ(irq); }

  static void disable() { asm("CPSID i"); }
  static void disableIRQ(IRQn_Type irq) { NVIC_DisableIRQ(irq); }
};
