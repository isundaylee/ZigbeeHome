/* blink.lds
 * linker script for blink demo.
 *
 * Memory layout on my STM32F103 is pretty much like this:
 *
 * 0x00000000 - 0x07ffffff - aliased to flash or sys memory depending on BOOT jumpers
 * 0x08000000 - 0x0800ffff - Flash (64K ... or 128K)
 * 0x1ffff000 - 0x1ffff7ff - Boot firmware in system memory
 * 0x1ffff800 - 0x1fffffff - option bytes
 * 0x20000000 - 0x20004fff - SRAM (20k)
 * 0x40000000 - 0x40023400 - peripherals
 */


MEMORY
{
    sram(WAIL) : ORIGIN = 0x20000000, LENGTH = 20K
    flash(RX)  : ORIGIN = 0x00000000, LENGTH = 128K
}

SECTIONS
{
    .text :
    {
        build/Boot.o (.text)
        * (.text)
    } > flash

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        * (.bss)
        . = ALIGN(4);
        _ebss = .;
    } > sram

    .init_array :
    {
        _init_array_start = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        _init_array_end = .;
    } > flash
}
