/*
 * liker_script.ld
 *
 *  Created on: 11 wrz 2018
 *      Author: mlokc
 */

INCLUDE memory_layout.ldh

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x20018000;    /* end of RAM */

_Min_Heap_Size = 0;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

/* Memories definition */
MEMORY
{
  RAM_EXCHANGE(rw) : ORIGIN = RAM_EXCHANGE_START, LENGTH = RAM_EXCHANGE_SIZE
  RAM (xrw)		: ORIGIN = RAM_EXCHANGE_START + RAM_EXCHANGE_SIZE, LENGTH = RAM_SIZE - RAM_EXCHANGE_SIZE
  ROM (rx)		: ORIGIN = BOOT_START, LENGTH = BOOT_SIZE
}

/* Sections */
SECTIONS
{
  /* The startup code into ROM memory */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >ROM

  .boot_startup :
  {
      . = ALIGN(4);
      KEEP(*(.boot_startup))
      . = ALIGN(4);
  } > ROM

  /* The program code and other data into ROM memory */
  .ram_code :
  {
    . = ALIGN(4);
    __ram_code_start__ = .;
    _ram_code_start = .;

    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
    __ram_code_end__ = .;
    _ram_code_end = .;
  } >RAM AT> ROM

  _ram_code_load = LOADADDR(.ram_code);

  /* Constant data into ROM memory*/
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >ROM

  .ARM.extab (READONLY) : { 
  	. = ALIGN(4);
  	*(.ARM.extab* .gnu.linkonce.armextab.*)
  	. = ALIGN(4);
  } >ROM
  
  .ARM (READONLY) : {
    . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(4);
  } >ROM

  .preinit_array (READONLY) :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4);
  } >ROM
  
  .init_array (READONLY) :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4);
  } >ROM
  
  .fini_array (READONLY) :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4);
  } >ROM

  .data_exchange_ram (NOLOAD) :
  {
      . = ALIGN(8);
      __data_exchange_start__ = .;
      KEEP(*(.data_exchange_section_ram))
      . = ALIGN(8);
      __data_exchange_end__ = .;
  } > RAM_EXCHANGE

  ASSERT(
      SIZEOF(.data_exchange_ram) <= LENGTH(RAM_EXCHANGE),
      "data_exchange RAM overflow"
  )

  /* Used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections into RAM memory */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM AT> ROM

  
  /* Uninitialized data section into RAM memory */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM

  /* Remove information from the compiler libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
