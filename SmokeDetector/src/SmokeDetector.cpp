#define DELAY(iterations) for (int i = 0; i < iterations; i++)

void setupClock() {
  // while ((RCC->CR & RCC_CR_HSIRDY) == 0)
  //   ;
  //
  // RCC->CFGR |= (0b0110 << 18);
  // RCC->CR |= RCC_CR_PLLON;
  // RCC->CFGR |= 0b10;
}

extern "C" void startup(void) { setupClock(); }
