/* locore.s
 * Assembler startup file for the STM32F103
 * Tom Trebisky  9-24-2016
 */

.cpu cortex-m0
.thumb

/* 0x00-C */
.word   0x20000800
.word   _reset
.word   spin
.word   spin2

/* 0x10-C */
.word   spin
.word   spin
.word   spin
.word   spin

/* 0x20-C */
.word   spin
.word   spin
.word   spin
.word   spin

/* 0x30-C */
.word   spin
.word   spin
.word   spin
.word   vector_systick

/* 0x40-C */
.word   spin
.word   spin
.word   spin
.word   spin

/* 0x50-C */
.word   spin
.word   spin
.word   spin
.word   spin

/* 0x60-C */
.word   spin
.word   spin
.word   spin
.word   spin

/* 0x70-C */
.word   spin
.word   spin
.word   spin
.word   spin

/* 0x80-C */
.word   spin
.word   spin
.word   spin
.word   spin

/* 0x90-C */
.word   spin
.word   spin
.word   spin
.word   spin

/* 0xA0-C */
.word   spin
.word   spin
.word   spin
.word   spin

/* 0xB0-C */
.word   vector_usart2
.word   spin
.word   spin
.word   spin

.thumb_func
spin:   b spin

.thumb_func
spin2:  b spin2

.thumb_func
_reset:
    bl startup
    b .
