//
// Created by Davide Collovigh on 25/10/24.
//

#include "load_pkt_from_mem.h"

const uint64_t ebpf_pkt_mem_base = 0;

int print_curr_pkt(uint16_t *curr, int count)
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
                print_curr_pkt(curr, count);
            }

            end_seq_cnt = 0;
        }

        count = print_curr_pkt(curr, count);
        curr++;
    } while (end_seq_cnt < STOP_SEQ_NO);

    uart_putc('\n');

    printf("END: 0x%16x\n", curr);
    return (void *) curr;
}

void load_packet_arr_from_mem(const void *region_start, const void *region_end, struct xdp_md *packets, int array_size)
{
    void *curr_start, *curr_end;
    curr_start = region_start;

    for(int k = 0; k < array_size; k++) {

        struct xdp_md *curr = &packets[k];

        printf("\nCOMPUTING SIZE x PACKET # %d\n", k);

        const void *pkt_start = curr_start;
        const void *pkt_end = get_next_pkt_end(curr_start, region_end);

        if (((uint64_t) pkt_start < ebpf_pkt_mem_base) || ((uint64_t) pkt_end < ebpf_pkt_mem_base)) {

          printf("ERROR: packet start or end (%16x, %16x) are lower than base (%16x)\n", pkt_start, pkt_end, ebpf_pkt_mem_base);
        }

        curr->data = (__u32) (pkt_start - ebpf_pkt_mem_base);
        curr->data_end = (__u32) (pkt_end - ebpf_pkt_mem_base);
        curr->ingress_ifindex = 99;
        curr->rx_queue_index = 0;
        curr->egress_ifindex = 0;
        curr->data_meta = 0;

        curr_start = pkt_end;
    }

    printf("\n\n\n");
}