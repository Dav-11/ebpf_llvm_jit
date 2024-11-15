//
// Created by Davide Collovigh on 26/09/24.
//

#include "../../rv64_baremetal_runtime/qemu_rv_uart.h"
#include "../../rv64_baremetal_runtime/bpf_helpers.h"
#include "../../rv64_baremetal_runtime/load_pkt_from_mem.h"

/**************************
 * MEM MAPPINGS
 **************************/

// specific for RV64 qemu
volatile char *uart_base = (volatile char *) UART0_BASE;

/**************************
 * ERRORS
 **************************/

#define E_END_REGION -3
#define BUF_SIZE 50
#define ARR_SIZE 1

int main() {
    UART0_FCR = UARTFCR_FFENA;    // Set the FIFO for polled operation
 uart_puts("Started runtime\n");  // Write the string to the UART

 // print region location
 printf("Start MM Region: 0x%x \n", &my_data_region_start);
 printf("End MM Region: 0x%x \n", &my_data_region_end);

 // test load pkt from mem
 struct xdp_md packet[ARR_SIZE];
 load_packet_arr_from_mem(&my_data_region_start, &my_data_region_end, packet, ARR_SIZE);

 for(int p = 0; p < ARR_SIZE; p++) {

  printf("EXECUTING XDP code on pkt # %d...\n", p);
  int ret = bpf_main(&packet[p], sizeof(struct xdp_md));
  printf("XDP result: %d\n\n", ret);
 }

 uart_puts("\n");

 while (1); // Loop forever to prevent program from ending
}