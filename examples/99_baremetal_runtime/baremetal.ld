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
    rodata_bpf_end = .;

    .rodata : { *(.rodata); *(.rodata.*) }

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

        SHORT(0x000c);
        SHORT(0x29aa);
        SHORT(0xfdff);
        SHORT(0x00e0);
        SHORT(0x4c68);
        SHORT(0x0654);
        SHORT(0x0800);
        SHORT(0x4500);

        SHORT(0x0034);
        SHORT(0x0000);
        SHORT(0x4000);
        SHORT(0x4006);
        SHORT(0xb3c6);
        SHORT(0xc0a8);
        SHORT(0x02b9);
        SHORT(0xc0a8);

        SHORT(0x02f4);
        SHORT(0xcff8);
        SHORT(0x0016);
        SHORT(0xe37f);
        SHORT(0xedf3);
        SHORT(0xa990);
        SHORT(0x2caa);
        SHORT(0x8010);

        SHORT(0x07fe);
        SHORT(0xce2e);
        SHORT(0x0000);
        SHORT(0x0101);
        SHORT(0x080a);
        SHORT(0x4dec);
        SHORT(0xa5bc);
        SHORT(0x452b);

        SHORT(0x6901);

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

        SHORT(0x48a9);
        SHORT(0x8a34);
        SHORT(0x52ba);
        SHORT(0x000c);
        SHORT(0x29aa);
        SHORT(0xfdff);
        SHORT(0x0800);
        SHORT(0x4500);

        SHORT(0x0048);
        SHORT(0x135c);
        SHORT(0x4000);
        SHORT(0x4011);
        SHORT(0x539d);
        SHORT(0xc0a8);
        SHORT(0x02f4);
        SHORT(0x0808);

        SHORT(0x0808);
        SHORT(0x86e3);
        SHORT(0x0035);
        SHORT(0x0034);
        SHORT(0xd3f1);
        SHORT(0x5836);
        SHORT(0x0100);
        SHORT(0x0001);

        SHORT(0x0000);
        SHORT(0x0000);
        SHORT(0x0000);
        SHORT(0x0331);
        SHORT(0x3835);
        SHORT(0x0132);
        SHORT(0x0331);
        SHORT(0x3638);

        SHORT(0x0331);
        SHORT(0x3932);
        SHORT(0x0769);
        SHORT(0x6e2d);
        SHORT(0x6164);
        SHORT(0x6472);
        SHORT(0x0461);
        SHORT(0x7270);

        SHORT(0x6100);
        SHORT(0x000c);
        SHORT(0x0001);

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
    . += 16K;          /* Allocate 16 KB for the stack */
    __stack_top = .;  /* Stack top symbol */
}
