//
// Created by Davide Collovigh on 26/09/24.
//

#include "qemu_rv_uart.h"
#include "bpf_helpers.h"

// specific for RV64 qemu
volatile char *uart_base = (volatile char *) 0x10000000;

// just declared, will be provided in link phase
int bpf_main(void* ctx, uint64_t size);

void main() {
    UART0_FCR = UARTFCR_FFENA;            // Set the FIFO for polled operation
    uart_puts("Hello World!\n");      // Write the string to the UART

    char str[] = "Hello: %d\n";
    _bpf_helper_ext_0006((uint64_t) str,11 ,6);


    char str2[20] = "Hello: %d\n";
    _bpf_helper_ext_0006((uint64_t) str2,20 ,9);

    while (1);                            // Loop forever to prevent program from ending
}