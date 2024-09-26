//
// Created by Davide Collovigh on 26/09/24.
//

#include "bpf_helpers.h"
#include "qemu_rv_uart.h"

// Function to handle format string and variable arguments
uint64_t _bpf_helper_ext_0006(uint64_t fmt, uint64_t fmt_size, ...) {
    va_list args;
    va_start(args, fmt_size);

    const char* format = (const char*)fmt;
    char buffer[32];

    for (int i = 0; i < fmt_size; i++) {
        if (format[i] == '%') {
            i++;
            if (format[i] == 'd') { // Integer
                int val = va_arg(args, int);
                itoa(val, buffer);
                uart_puts(buffer);
            } else if (format[i] == 'x') { // Hexadecimal
                uint64_t val = va_arg(args, uint64_t);
                hextoa(val, buffer);
                uart_puts(buffer);
            } else if (format[i] == 's') { // String
                const char* str = va_arg(args, const char*);
                uart_puts(str);
            } else if (format[i] == 'c') { // Character
                char c = (char)va_arg(args, int);
                uart_putc(c);
            }
        } else {
            uart_putc(format[i]); // Print regular characters
        }
    }

    va_end(args);
    return 0;
}
