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
.word   spin

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
.word   spin

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
.word   vector_usart1

spin:   b spin

.thumb_func
_reset:
    bl startup
    b .
