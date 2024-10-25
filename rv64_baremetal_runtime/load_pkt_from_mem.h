//
// Created by Davide Collovigh on 25/10/24.
//

#ifndef EBPF_LLVM_JIT_LOAD_PKT_FROM_MEM_H
#define EBPF_LLVM_JIT_LOAD_PKT_FROM_MEM_H

#include "qemu_rv_uart.h"
#include "bpf_helpers.h"

#define STOP_SEQ 0xFFFF
#define STOP_SEQ_NO 4

void load_packet_from_mem(void *region_start, void *region_end, struct xdp_md *pkt);
void load_packet_arr_from_mem(const void *region_start, const void *region_end, struct xdp_md *packets, int array_size);

#endif //EBPF_LLVM_JIT_LOAD_PKT_FROM_MEM_H
