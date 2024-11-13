
#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#define BASE_ADDRESS_QEMU_RV64 0x0000000080000000

/**************************
 * MEM MAPPINGS
 **************************/

// data region where pkt are stored
extern const char my_data_region_start;
extern const char my_data_region_end;

// .rodata.bpf section
extern const char rodata_bpf_start;
extern const char rodata_str1_bpf_start;
extern const char rodata_bpf_end;

// stack
extern const char __stack_bottom;
extern const char __stack_top;

// BASE for
const uint64_t ebpf_pkt_mem_base = 0;

#endif //MEMORY_H