//
// Created by Davide Collovigh on 26/09/24.
//

#include "qemu_rv_uart.h"

void uart_putc(char c) {
    while (!UART0_FF_THR_EMPTY);            // Wait until the FIFO holding register is empty
    UART0_DR = c;                           // Write character to transmitter register
}

void uart_puts(const char *str) {
    while (*str) {                          // Loop until value at string pointer is zero
        uart_putc(*str++);                    // Write the character and increment pointer
    }
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

void hextoa(uint64_t n, char* buffer)
{
    const char digits[] = "0123456789ABCDEF";
    char* ptr = buffer;
    char* start = buffer;
    int base = 16;

    if (n == 0) {

        *ptr++ = '0';
    } else {

        while (n > 0) {
            *ptr++ = digits[n % base];
            n /= base;
        }

        *ptr = '\0';
    }

    // Reverse the string in-place
    char* end = ptr - 1;
    while (start < end) {
        char temp = *start;
        *start++ = *end;
        *end-- = temp;
    }
}
