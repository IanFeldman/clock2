ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 16K
  RAM (rwx)  : ORIGIN = 0x10000000, LENGTH = 4K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
  . = ORIGIN(FLASH);
  .isr_vector (READONLY) :
  {
    KEEP(*(.isr_vector))
  } > FLASH

  .text (READONLY) :
  {
    *(.text*)
    *(.rodata*)
  } > FLASH

  . = ORIGIN(RAM);

  .data : AT (ADDR(.text) + SIZEOF(.text))
  {
    _sidata = LOADADDR(.data);
    _sdata = .;
    *(.data*)
    _edata = .;
  } > RAM

  .bss :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM
}

