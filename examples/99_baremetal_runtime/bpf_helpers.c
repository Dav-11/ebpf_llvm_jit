//
// Created by Davide Collovigh on 26/09/24.
//

#include "bpf_helpers.h"
#include "qemu_rv_uart.h"

// Function to handle format string and variable arguments
uint64_t _bpf_helper_ext_0006(const char *fmt, uint64_t fmt_size, ...) {

    char *fmt_str = fmt;
    char *fmt_relocated = (void *)(&rodata_bpf_start) + (uint64_t) fmt;
    uint64_t rodata_len = (void *)(&rodata_bpf_end) - (void *)(&rodata_bpf_start);

    // printf("FMT:\t\t0x%16x\n", fmt);
    // printf("RELOCATED:\t0x%16x\n", fmt_relocated);
    // printf("rodata_start:\t0x%16x\n", (void *)(&rodata_bpf_start));
    // printf("rodata_end:\t0x%16x\n", (void *)(&rodata_bpf_end));
    // printf("rodata_len: %d\n", rodata_len);

    // char *str = (void *)(&rodata_bpf_start);
    // for(int i = 0; i < rodata_len; i++) {
    //     printf("%2x", str[i]);
    // }

    // uart_putc('\n');


    if(fmt_str == NULL) {

        // try to check if the string is into .rodata.bpf
        if (fmt_relocated != NULL) {

            fmt_str = fmt_relocated;
            // printf("%s\n", fmt_relocated);
        } else {

            uart_puts("called _bpf_helper_ext_0006 with empty pointer as fmt\n");
            return 0;
        }
    }

    va_list args;
    va_start(args, fmt_size);

    const char* format = (const char*)fmt_str;
    char buffer[32];
    int len = 0;
    int i = 0;
    unsigned int ascii_code = 0;

    // S0: print chars until '\0' or '%'
s0:

    if (format[i] == '%') {
        i++;
        goto s1;
    }

    if (format[i] == '\0')
        goto end;

    if (i >= fmt_size)
      goto end;

    uart_putc(format[i]);
    i++;
    goto s0;

    // s1: if exists -> read number of char to use for printing decimal/hex
s1:

    ascii_code = format[i];
    if ((ascii_code - '0' >= 0) && (ascii_code - '0' < 10)) {
        int value = ascii_code - '0';
        len = (len *10) + value;
        i++;
        goto s1;
    }

    goto s2;

    // s2: read param type and convert param to string
s2:

    if (format[i] == 'd') { // Integer
        int val = va_arg(args, int);
        itoa(val, buffer);
        uart_puts(buffer);
    } else if (format[i] == 'x') { // Hexadecimal
        uint32_t val = va_arg(args, uint32_t);
        hextoa(val, buffer, len);
        uart_puts(buffer);
    } else if (format[i] == 's') { // String
        const char* str = va_arg(args, const char*);
        uart_puts(str);
    } else if (format[i] == 'c') { // Character
        char c = (char)va_arg(args, int);
        uart_putc(c);
    }

    len = 0; // reset len
    i++;

    goto s0;

end:

    va_end(args);
    return 0;
}
