//
// Created by Davide Collovigh on 26/09/24.
//

#ifndef BAREMETAL_RV_QEMU_RV_UART_H
#define BAREMETAL_RV_QEMU_RV_UART_H

#include <stdint.h>

#define UART0_BASE 0x10000000

// Use a datasheet for a 16550 UART
// For example: https://www.ti.com/lit/ds/symlink/tl16c550d.pdf
#define REG(base, offset) ((*((volatile unsigned char *)(base + offset))))
#define UART0_DR    REG(UART0_BASE, 0x00)
#define UART0_FCR   REG(UART0_BASE, 0x02)
#define UART0_LSR   REG(UART0_BASE, 0x05)

#define UARTFCR_FFENA 0x01                // UART FIFO Control Register enable bit
#define UARTLSR_THRE 0x20                 // UART Line Status Register Transmit Hold Register Empty bit
#define UART0_FF_THR_EMPTY (UART0_LSR & UARTLSR_THRE)

void uart_putc(char c);
void uart_puts(const char *str);

void reverse_string(char str[], int length);
void itoa(int n, char s[]);
void hextoa(uint64_t n, char* buffer);

#endif //BAREMETAL_RV_QEMU_RV_UART_H
