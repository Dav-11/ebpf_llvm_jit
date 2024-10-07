
#ifndef MEMORY_H
#define MEMORY_H

#define BASE_ADDRESS_QEMU_RV64 0x0000000080000000

/**************************
 * MEM MAPPINGS
 **************************/

// data region where pkt are stored
extern const char my_data_region_start;
extern const char my_data_region_end;

// .rodata.bpf section
extern const char rodata_bpf_start;
extern const char rodata_bpf_end;


#endif //MEMORY_H