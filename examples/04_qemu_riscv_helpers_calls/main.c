//
// Created by Davide Collovigh on 26/09/24.
//

#include "../99_baremetal_runtime/qemu_rv_uart.h"
#include "../99_baremetal_runtime/bpf_helpers.h"

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
#define BUF_SIZE 50

int load_packet_from_mem(void *region_start, void *region_end, struct xdp_md *packet)
{
    packet->data = (__u32) (region_start - my_data_region_start);

    __u64 *prev = (__u64 *)region_start;
    __u64 *curr = (__u64 *)region_start;
    __u64 pkt_end = 0;

    int count = 0;

    uart_puts("++++ Packet content ++++\n");

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
        printf(str, curr, *curr);

      prev = curr;
        curr++;
        count++;
    }

    // Set pkt_end to the address right before the 64-bit sequence of 1s
    packet->data_end = (__u32)(prev - my_data_region_start);

    // set values to default
    packet->ingress_ifindex = 99;
    packet->rx_queue_index = 0;
    packet->egress_ifindex = 0;
    packet->data_meta = 0;

    uart_puts("\n++++ END +++++\n\n");
}


void main() {
    UART0_FCR = UARTFCR_FFENA;    // Set the FIFO for polled operation
    uart_puts("Started runtime\n");  // Write the string to the UART

    char str[BUF_SIZE] = "initialized";

    // test bpf_printk();

//    strcpy(str, "Hello: %d\n");
//    printf(str, 6);
//
//    strcpy(str, "Hello: %d\n");
//    printf(str, 9);

    // print region location
    strcpy(str, "Start MM Region: 0x%x \n");
    printf(str, &my_data_region_start);

    strcpy(str, "End MM Region: 0x%x \n");
    printf(str, &my_data_region_end);

    // test load pkt from mem
    struct xdp_md packet;
    load_packet_from_mem(&my_data_region_start, &my_data_region_end, &packet);

    strcpy(str, "Start Pkt: 0x%x\n");
   printf(str, packet.data);

    strcpy(str, "End Pkt: 0x%x\n");
    printf(str, packet.data_end);

    int ret = bpf_main(&packet, sizeof(packet));

    printf("XDP result: %d\n", ret);

    while (1); // Loop forever to prevent program from ending
}