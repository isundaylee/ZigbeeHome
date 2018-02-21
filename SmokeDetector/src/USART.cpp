#include "USART.h"

extern "C" void vector_usart2() {
  while (USART_2::usart()->ISR & USART_ISR_RXNE) {
    USART_2::rxBuffer_.push((uint8_t)USART_2::usart()->RDR);
  }
}
