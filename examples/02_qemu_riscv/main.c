//
// Created by Davide Collovigh on 26/09/24.
//

#include "qemu_rv_uart.h"
#include "bpf_helpers.h"

/**************************
 * MEM MAPPINGS
 **************************/

// specific for RV64 qemu
volatile char *uart_base = (volatile char *) UART0_BASE;

// data region where pkt are stored
extern uint32_t my_data_region_start;
extern uint32_t my_data_region_end;

/**************************
 * ERRORS
 **************************/

#define E_END_REGION -3

int load_packet_from_mem(void *region_start, void *region_end, struct xdp_md *packet)
{
    packet->data = (__u32)region_start;

    __u32 *prev = (__u32 *)region_start;
    __u32 *curr = (__u32 *)region_start;
    __u32 pkt_end = 0;

    int count = 0;

    // Scan the memory for a 64-bit sequence of all 1s
    while (*curr != 0xFFFFFFFF || *prev != 0xFFFFFFFF) {

      if (curr == region_end) {

            uart_puts("hit mem zone end\n");
            return E_END_REGION;
        }

        if (count % 4 == 0) {
            uart_puts("\n");
        }

        char str[20] = "[0x%8x]: 0x%8x\n";
        _bpf_helper_ext_0006(str,20 ,curr, *curr);

      prev = curr;
        curr++;
        count++;
    }

    // Set pkt_end to the address right before the 64-bit sequence of 1s
    pkt_end = (__u32)prev;

    packet->data_end = (__u64)pkt_end;
}


void main() {
    UART0_FCR = UARTFCR_FFENA;    // Set the FIFO for polled operation
    uart_puts("Hello World!\n");  // Write the string to the UART

    // test bpf_printk();

    char str[20] = "Hello: %d\n";
    _bpf_helper_ext_0006(str,20 ,6);

    strcpy(str, "Hello: %d\n");
    _bpf_helper_ext_0006(str,20 ,9);

    // print region location
    strcpy(str, "Start Region: 0x%x\n");
    _bpf_helper_ext_0006(str,20 ,&my_data_region_start);

    strcpy(str, "End Region: 0x%x\n");
    _bpf_helper_ext_0006(str,20 ,&my_data_region_end);

    // test load pkt from mem
    struct xdp_md packet;
    load_packet_from_mem(&my_data_region_start, &my_data_region_end, &packet);

    strcpy(str, "Start Pkt: 0x%x\n");
   _bpf_helper_ext_0006(str,20 ,packet.data);

    strcpy(str, "End Pkt: 0x%x\n");
    _bpf_helper_ext_0006(str,20 ,packet.data_end);


    while (1); // Loop forever to prevent program from ending
}