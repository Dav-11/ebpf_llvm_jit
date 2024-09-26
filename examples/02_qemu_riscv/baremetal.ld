SECTIONS
{
    /* QEMU default load address to run bios */
    . = 0x80000000;

    /* Define an output section ".text". */
    .text : {

        /* Ensure _start is placed first */
        KEEP(*(.text._start));

        /* Pull in all symbols in input sections named .text */
        *(.text)

        /* Do the same for sections starting with .text., such as .text.foo */
        *(.text.*)
    }

    /* Make sure linker does not jam data into text section, making text writable */
    . = ALIGN (CONSTANT (COMMONPAGESIZE));

    /* Do the same for ".bss", ".rodata", and ".data". */
    .bss : { *(.bss); *(.bss.*) }
    .data : { *(.data); *(.data.*) } /* Stack is placed here */
    .rodata : { *(.rodata); *(.rodata.*) }

    /* Place my_data_region after .data section
     *
     * C:
     * extern uint32_t my_data_region;
     * uint32_t first_value = *(uint32_t *)&my_data_region;
     *
     * ASM:
     * la t0, my_data_region  # Load the address of my_data_region
     * lw t1, 0(t0)           # Load the first 32-bit value (0xDEADBEEF)
     *
     */
    . = ALIGN(16);   /* Align to 16 bytes */
    my_data_region : {
        LONG(0xDEADBEEF);   /* Example 32-bit value */
        LONG(0x12345678);   /* Another 32-bit value */
        SHORT(0xABCD);      /* Example 16-bit value */
        BYTE(0x42);         /* Example 8-bit value */
    }

    /* Stack allocation */
    . = ALIGN(16);    /* Align the stack */
    __stack_bottom = .;        /* Stack bottom symbol */
    . += 16K;          /* Allocate 16 KB for the stack */
    __stack_top = .;  /* Stack top symbol */
}
