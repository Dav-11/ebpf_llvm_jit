//
// Created by Davide Collovigh on 26/09/24.
//

#include "../../rv64_baremetal_runtime/load_pkt_from_mem.h"
#include "../../rv64_baremetal_runtime/memory.h"
#include "../../rv64_baremetal_runtime/qemu_rv_uart.h"

// specific for RV64 qemu
volatile char *uart_base = (volatile char *) UART0_BASE;

int main() {
    UART0_FCR = UARTFCR_FFENA;    // Set the FIFO for polled operation
    uart_puts("Started runtime\n");  // Write the string to the UART

    printf("Start MM Region: 0x%x \n", &my_data_region_start);
    printf("End MM Region: 0x%x \n", &my_data_region_end);

    // test load pkt from mem
    struct xdp_md packet;
    load_packet_from_mem(&my_data_region_start, &my_data_region_end, &packet);

    printf("Start Pkt: 0x%x\n", packet.data);
    printf("End Pkt: 0x%x\n", packet.data_end);

    int ret = bpf_main(&packet, sizeof(packet));

    printf("XDP result: %d\n", ret);

    while (1); // Loop forever to prevent program from ending
}