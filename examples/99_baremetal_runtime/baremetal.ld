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
    my_data_region_start = .; /* Declare the start of the region */
    my_data_region : {

        /*
            ---- PKT 1 ----

            15:48:50.177524 IP 192.168.2.185.53240 > 192.168.2.244.ssh: Flags [.], ack 377, win 2046, options [nop,nop,TS val 1307354556 ecr 1160472833], length 0
            	0x0000:  000c 29aa fdff 00e0 4c68 0654 0800 4500  ..).....Lh.T..E.
            	0x0010:  0034 0000 4000 4006 b3c6 c0a8 02b9 c0a8  .4..@.@.........
            	0x0020:  02f4 cff8 0016 e37f edf3 a990 2caa 8010  ............,...
            	0x0030:  07fe ce2e 0000 0101 080a 4dec a5bc 452b  ..........M...E+
            	0x0040:  6901                                     i.
        */
        LONG(0x000c29aa);   /* 32-bit value */
        LONG(0xfdff00e0);
        LONG(0x4c680654);
        LONG(0x08004500);

        LONG(0x00340000);
        LONG(0x40004006);
        LONG(0xb3c6c0a8);
        LONG(0x02b9c0a8);

        LONG(0x02f4cff8);
        LONG(0x0016e37f);
        LONG(0xedf3a990);
        LONG(0x2caa8010);

        LONG(0x07fece2e);
        LONG(0x00000101);
        LONG(0x080a4dec);
        LONG(0xa5bc452b);

        LONG(0x69010000);

        /* CUSTOM END SEQ */
        QUAD(0xFFFFFFFFFFFFFFFF);

        /*
            ---- PKT 2 ----

            15:48:50.194311 IP 192.168.2.244.34531 > dns.google.domain: 22582+ PTR? 185.2.168.192.in-addr.arpa. (44)
            	0x0000:  48a9 8a34 52ba 000c 29aa fdff 0800 4500  H..4R...).....E.
            	0x0010:  0048 135c 4000 4011 539d c0a8 02f4 0808  .H.\@.@.S.......
            	0x0020:  0808 86e3 0035 0034 d3f1 5836 0100 0001  .....5.4..X6....
            	0x0030:  0000 0000 0000 0331 3835 0132 0331 3638  .......185.2.168
            	0x0040:  0331 3932 0769 6e2d 6164 6472 0461 7270  .192.in-addr.arp
            	0x0050:  6100 000c 0001                           a.....
         */

        QUAD(0x48a98a3452ba000c);
        QUAD(0x29aafdff08004500);

        QUAD(0x0048135c40004011);
        QUAD(0x539dc0a802f40808);

        QUAD(0x080886e300350034);
        QUAD(0xd3f1583601000001);

        QUAD(0x0000000000000331);
        QUAD(0x3835013203313638);

        QUAD(0x0331393207696e2d);
        QUAD(0x6164647204617270);

        QUAD(0x6100000c00010000);

        /* CUSTOM END SEQ */
        QUAD(0xFFFFFFFFFFFFFFFF);
    }
    my_data_region_end = .; /* declare the end of the data region */

    /* Stack allocation */
    . = ALIGN(16);    /* Align the stack */
    __stack_bottom = .;        /* Stack bottom symbol */
    . += 16K;          /* Allocate 16 KB for the stack */
    __stack_top = .;  /* Stack top symbol */
}
