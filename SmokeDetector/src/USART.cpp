#include "USART.h"

extern "C" void vector_usart2() {
  while (USART_2::usart()->ISR & USART_ISR_RXNE) {
    USART_2::rxBuffer_.push(static_cast<uint8_t>(USART_2::usart()->RDR));
  }

  if (USART_2::usart()->ISR & USART_ISR_ORE) {
    USART_2::flush();
    USART_2::usart()->ICR |= USART_ICR_ORECF;
  }
}
