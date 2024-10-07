//
// Created by Davide Collovigh on 26/09/24.
//

#include "../99_baremetal_runtime/qemu_rv_uart.h"
#include "../99_baremetal_runtime/bpf_helpers.h"
//#include "../99_baremetal_runtime/memory.h"

#define BUF_SIZE 40
#define ARR_SIZE 2
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

void *get_next_pkt_end(const void *region_start, const void *region_end)
{
    uint16_t *curr = (uint16_t *)region_start;
    int count = 0;
    int end_seq_cnt = 0;

    do {

        if (curr == region_end) {

            uart_puts("hit mem zone end\n");
            return NULL;
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

    printf("END: 0x%16x\n", curr);
    return (void *) curr;
}

int load_packet_arr_from_mem(const void *region_start, const void *region_end, struct xdp_md *packets)
{
    void *curr_start, *curr_end;
    curr_start = region_start;

    for(int k = 0; k < ARR_SIZE; k++) {

        struct xdp_md *curr = &packets[k];

        printf("\nCOMPUTING SIZE x PACKET # %d\n", k);

        curr->data = (__u32) curr_start;
        curr->data_end = (__u32) get_next_pkt_end(curr_start, region_end);
        curr->ingress_ifindex = 99;
        curr->rx_queue_index = 0;
        curr->egress_ifindex = 0;
        curr->data_meta = 0;

        curr_start = curr->data_end;
    }

    printf("\n\n\n");
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
    struct xdp_md packet[ARR_SIZE];
    load_packet_arr_from_mem(&my_data_region_start, &my_data_region_end, packet);

    for(int p = 0; p < ARR_SIZE; p++) {

        printf("EXECUTING XDP_MD # %d...\n", p);
        int ret = bpf_main(&packet[p], sizeof(struct xdp_md));
        printf("XDP result: %d\n\n", ret);
    }

    uart_puts("\n");

    while (1); // Loop forever to prevent program from ending
}