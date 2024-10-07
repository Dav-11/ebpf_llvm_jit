//
// Created by Davide Collovigh on 26/09/24.
//

#include "../99_baremetal_runtime/qemu_rv_uart.h"
#include "../99_baremetal_runtime/bpf_helpers.h"
//#include "../99_baremetal_runtime/memory.h"


#define BUF_SIZE 50
#define STOP_SEQ 0xFFFF
#define STOP_SEQ_NO 4

// specific for RV64 qemu
volatile char *uart_base = (volatile char *) UART0_BASE;

int print_curr(uint16_t *curr, int count)
{

    if (count % 16 == 0) {
        printf("\n[0x%16x]:", curr);
    }

    printf(" %4x", *curr);
    count++;

    return count;
}

int load_packet_from_mem(const void *region_start, const void *region_end, struct xdp_md *packet)
{

    uint32_t offset_start = (region_start - my_data_region_start);
    packet->data = offset_start;

    printf("packet->data = 0x%8x\n", offset_start);

    //unsigned char *prev = (unsigned char *)region_start;
    uint16_t *curr = (uint16_t *)region_start;
    __u64 pkt_end = 0;

    int count = 0;
    int end_seq_cnt = 0;

    uart_puts("++++ Packet content ++++\n");

    // Scan the memory for a 64-bit sequence of all 1s
    do {

        if (curr == region_end) {

            uart_puts("hit mem zone end\n");
            return E_END_REGION;
        }

        if (*curr == STOP_SEQ) {

            // just increment but do not print
            end_seq_cnt++;
            curr++;
            continue;

        } else if (end_seq_cnt != 0) {

            for (int i =0; i< end_seq_cnt; i++) {

                // print skipped sequence of 0xFFFF
                print_curr(curr, count);
            }

            end_seq_cnt = 0;
        }

        count = print_curr(curr, count);
        curr++;
    } while (end_seq_cnt < STOP_SEQ_NO);

    uart_putc('\n');

    // Set pkt_end to the address right before the 64-bit sequence of 1s
    packet->data_end = (__u32)(curr - my_data_region_start - 4);
    printf("packet->data_end = 0x%8x\n", packet->data_end);

    // set values to default
    packet->ingress_ifindex = 99;
    packet->rx_queue_index = 0;
    packet->egress_ifindex = 0;
    packet->data_meta = 0;

    uart_puts("\n\n++++ END +++++\n\n");
}


void main() {
    UART0_FCR = UARTFCR_FFENA;    // Set the FIFO for polled operation
    uart_puts("Started runtime\n");  // Write the string to the UART

    char str[BUF_SIZE] = "initialized";

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