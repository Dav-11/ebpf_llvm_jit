//
// Created by Davide Collovigh on 26/09/24.
//

#ifndef BAREMETAL_RV_BPF_HELPERS_H
#define BAREMETAL_RV_BPF_HELPERS_H

#include <stdarg.h>
#include <stdint.h>
#include "memory.h"

typedef uint32_t __u32;
typedef uint64_t __u64;

/**
 * @brief structure that holds pointers to network packet
 * 
 */
struct xdp_md {
    __u32 data;
    __u32 data_end;
    __u32 data_meta;
    __u32 ingress_ifindex;  /* rxq->dev->ifindex */
    __u32 rx_queue_index;   /* rxq->queue_index  */
    __u32 egress_ifindex;   /* txq->dev->ifindex */
};

/**
 * @brief bpf program
 * 
 * @param ctx 
 * @param size 
 * @return int
 */
int bpf_main(void* ctx, uint64_t size);

/**
 * @brief bpf_printk - prints formatted text
 * 
 * @param fmt format string, it is always (AFAIK) a string from .rodata
 * @param fmt_size size of the format string
 * @param ... params
 * @return uint64_t 
 */
uint64_t _bpf_helper_ext_0006(const char *fmt, uint64_t fmt_size, ...);

#endif //BAREMETAL_RV_BPF_HELPERS_H
