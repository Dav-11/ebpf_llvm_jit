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
    
    /* Declare a symbol marking the start of the .rodata.bpf section */
    . = ALIGN(1);
    rodata_bpf_start = .;
    .rodata.bpf : {
        *(.rodata.bpf)
    }
    rodata_str1_bpf_start = .;
    .rodata.str1.1 : {
        *(.rodata.str1.1)
    }
    rodata_bpf_end = .;

    .rodata : { *(.rodata); *(.rodata.*) }

    . = ALIGN(1);   /* Align to 16 bytes */
    my_data_region_start = .; /* Declare the start of the region */
    my_data_region : {

        /*
            BYTE(0x0c);                 // 1 byte
            SHORT(0x29aa);              // 2 bytes
            WORD(0xfdff1234);           // 4 bytes (or LONG)
            QUAD(0x123456789abcdef0);   // 8 bytes
        */


        /*
            ---- PKT 1 ----

            15:48:50.177524 IP 192.168.2.185.53240 > 192.168.2.244.ssh: Flags [.], ack 377, win 2046, options [nop,nop,TS val 1307354556 ecr 1160472833], length 0
            	0x0000:  000c 29aa fdff 00e0 4c68 0654 0800 4500  ..).....Lh.T..E.
            	0x0010:  0034 0000 4000 4006 b3c6 c0a8 02b9 c0a8  .4..@.@.........
            	0x0020:  02f4 cff8 0016 e37f edf3 a990 2caa 8010  ............,...
            	0x0030:  07fe ce2e 0000 0101 080a 4dec a5bc 452b  ..........M...E+
            	0x0040:  6901                                     i.
        */

        /* 000c 29aa fdff 00e0 4c68 0654 0800 4500 */
        BYTE(0x00);
        BYTE(0x0C);
        BYTE(0x29);
        BYTE(0xAA);
        BYTE(0xFD);
        BYTE(0xFF);
        BYTE(0x00);
        BYTE(0xE0);
        BYTE(0x4C);
        BYTE(0x68);
        BYTE(0x06);
        BYTE(0x54);
        BYTE(0x08);
        BYTE(0x00);
        BYTE(0x45);
        BYTE(0x00);


        /* 0034 0000 4000 4006 b3c6 c0a8 02b9 c0a8 */
        BYTE(0x00);
        BYTE(0x34);
        BYTE(0x00);
        BYTE(0x00);
        BYTE(0x40);
        BYTE(0x00);
        BYTE(0x40);
        BYTE(0x06);
        BYTE(0xB3);
        BYTE(0xC6);
        BYTE(0xC0);
        BYTE(0xA8);
        BYTE(0x02);
        BYTE(0xB9);
        BYTE(0xC0);
        BYTE(0xA8);


        /* 02f4 cff8 0016 e37f edf3 a990 2caa 8010 */
        BYTE(0x02);
        BYTE(0xF4);
        BYTE(0xCF);
        BYTE(0xF8);
        BYTE(0x00);
        BYTE(0x16);
        BYTE(0xE3);
        BYTE(0x7F);
        BYTE(0xED);
        BYTE(0xF3);
        BYTE(0xA9);
        BYTE(0x90);
        BYTE(0x2C);
        BYTE(0xAA);
        BYTE(0x80);
        BYTE(0x10);


        /* 07fe ce2e 0000 0101 080a 4dec a5bc 452b */
        BYTE(0x07);
        BYTE(0xFE);
        BYTE(0xCE);
        BYTE(0x2E);
        BYTE(0x00);
        BYTE(0x00);
        BYTE(0x01);
        BYTE(0x01);
        BYTE(0x08);
        BYTE(0x0A);
        BYTE(0x4D);
        BYTE(0xEC);
        BYTE(0xA5);
        BYTE(0xBC);
        BYTE(0x45);
        BYTE(0x2B);


        /* 6901 */
        BYTE(0x69);
        BYTE(0x01);

        /* CUSTOM END SEQ */
        SHORT(0xFFFF);
        SHORT(0xFFFF);
        SHORT(0xFFFF);
        SHORT(0xFFFF);

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

        /* 48a9 8a34 52ba 000c 29aa fdff 0800 4500 */
        BYTE(0x48);
        BYTE(0xA9);
        BYTE(0x8A);
        BYTE(0x34);
        BYTE(0x52);
        BYTE(0xBA);
        BYTE(0x00);
        BYTE(0x0C);
        BYTE(0x29);
        BYTE(0xAA);
        BYTE(0xFD);
        BYTE(0xFF);
        BYTE(0x08);
        BYTE(0x00);
        BYTE(0x45);
        BYTE(0x00);


        /* 0048 135c 4000 4011 539d c0a8 02f4 0808 */
        BYTE(0x00);
        BYTE(0x48);
        BYTE(0x13);
        BYTE(0x5C);
        BYTE(0x40);
        BYTE(0x00);
        BYTE(0x40);
        BYTE(0x11);
        BYTE(0x53);
        BYTE(0x9D);
        BYTE(0xC0);
        BYTE(0xA8);
        BYTE(0x02);
        BYTE(0xF4);
        BYTE(0x08);
        BYTE(0x08);


        /* 0808 86e3 0035 0034 d3f1 5836 0100 0001 */
        BYTE(0x08);
        BYTE(0x08);
        BYTE(0x86);
        BYTE(0xE3);
        BYTE(0x00);
        BYTE(0x35);
        BYTE(0x00);
        BYTE(0x34);
        BYTE(0xD3);
        BYTE(0xF1);
        BYTE(0x58);
        BYTE(0x36);
        BYTE(0x01);
        BYTE(0x00);
        BYTE(0x00);
        BYTE(0x01);


        /* 0000 0000 0000 0331 3835 0132 0331 3638 */
        BYTE(0x00);
        BYTE(0x00);
        BYTE(0x00);
        BYTE(0x00);
        BYTE(0x00);
        BYTE(0x00);
        BYTE(0x03);
        BYTE(0x31);
        BYTE(0x38);
        BYTE(0x35);
        BYTE(0x01);
        BYTE(0x32);
        BYTE(0x03);
        BYTE(0x31);
        BYTE(0x36);
        BYTE(0x38);


        /* 0331 3932 0769 6e2d 6164 6472 0461 7270 */
        BYTE(0x03);
        BYTE(0x31);
        BYTE(0x39);
        BYTE(0x32);
        BYTE(0x07);
        BYTE(0x69);
        BYTE(0x6E);
        BYTE(0x2D);
        BYTE(0x61);
        BYTE(0x64);
        BYTE(0x64);
        BYTE(0x72);
        BYTE(0x04);
        BYTE(0x61);
        BYTE(0x72);
        BYTE(0x70);


        /* 6100 000c 0001 */
        BYTE(0x61);
        BYTE(0x00);
        BYTE(0x00);
        BYTE(0x0C);
        BYTE(0x00);
        BYTE(0x01);


        /* CUSTOM END SEQ */
        SHORT(0xFFFF);
        SHORT(0xFFFF);
        SHORT(0xFFFF);
        SHORT(0xFFFF);
    }
    my_data_region_end = .; /* declare the end of the data region */

    /* Stack allocation */
    . = ALIGN(16);    /* Align the stack */
    __stack_bottom = .;        /* Stack bottom symbol */
    . += 4M;          /* Allocate 16 KB for the stack */
    __stack_top = .;  /* Stack top symbol */
}
