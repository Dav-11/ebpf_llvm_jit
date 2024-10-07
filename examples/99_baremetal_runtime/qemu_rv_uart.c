//
// Created by Davide Collovigh on 26/09/24.
//
#include "qemu_rv_uart.h"
#include <stdarg.h>

void uart_putc(char c) {
    while (!UART0_FF_THR_EMPTY);            // Wait until the FIFO holding register is empty
    UART0_DR = c;                           // Write character to transmitter register
}

int uart_puts(const char *str) {

  int size = 0;

    while (*str) {                          // Loop until value at string pointer is zero
        uart_putc(*str++);                    // Write the character and increment pointer
        size++;
    }

    return size;
}


void reverse_string(char *buffer, int length)
{
    int i, j;
    char c;

    for (i = 0, j = length-1; i<j; i++, j--) {
        c = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = c;
    }
}

void itoa(int n, char* buffer)
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        buffer[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        buffer[i++] = '-';
    buffer[i] = '\0';
    reverse_string(buffer, i);
}

void hextoa(uint32_t n, char* buffer, int fixed_size)
{
    const char digits[] = "0123456789ABCDEF";
    char* ptr = buffer;
    char* start = buffer;
    int base = 16;

    int cnt = 0;



    if (n == 0) {

        *ptr++ = '0';
        cnt++;
    } else {

        while (n > 0) {
            *ptr++ = digits[n % base];
            n /= base;
            cnt++;
        }
    }

    // if fixed size -> add zeroes
    if (fixed_size > 0) {
        while (cnt < fixed_size) {

            *ptr++ = '0';
            cnt++;
        }
    }

    *ptr = '\0';

    // Reverse the string in-place
    char* end = ptr - 1;
    while (start < end) {
        char temp = *start;
        *start++ = *end;
        *end-- = temp;
    }
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while (*src) {
        *d++ = *src++;
    }
    *d = '\0';
    return dest;
}

uint64_t printf(const char *fmt, ...) {

    if(fmt == NULL) {
        uart_puts("called _bpf_helper_ext_0006 with empty pointer as fmt\n");
        return 0;
    }

    va_list args;
    va_start(args, fmt);

    const char* format = (const char*)fmt;
    char buffer[32];
    int len = 0;
    int i = 0;
    int written = 0;
    unsigned int ascii_code = 0;

    // S0: print chars until '\0' or '%'
s0:

    if (format[i] == '%') {
        i++;
        goto s1;
    }

    if (format[i] == '\0')
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
        written++;

    } else if (format[i] == 'x') { // Hexadecimal
        uint32_t val = va_arg(args, uint32_t);
        hextoa(val, buffer, len);
        written += uart_puts(buffer);

    } else if (format[i] == 's') { // String
        const char* str = va_arg(args, const char*);
        written += uart_puts(str);

    } else if (format[i] == 'c') { // Character
        char c = (char)va_arg(args, int);
        uart_putc(c);
        written++;
    }

    len = 0; // reset len
    i++;

    goto s0;

end:
    va_end(args);
    return written;
}

int strlen(const char *str)
{
    int i = 0;

    while(str[i] != '\0') {
        i++;
    }

    return i;
}
