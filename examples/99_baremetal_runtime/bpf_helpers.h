//
// Created by Davide Collovigh on 26/09/24.
//

#ifndef BAREMETAL_RV_BPF_HELPERS_H
#define BAREMETAL_RV_BPF_HELPERS_H

#include <stdarg.h>
#include <stdint.h>

typedef uint32_t __u32;
typedef uint64_t __u64;

struct xdp_md {
    __u32 data;
    __u32 data_end;
    __u32 data_meta;
    __u32 ingress_ifindex;  /* rxq->dev->ifindex */
    __u32 rx_queue_index;   /* rxq->queue_index  */
    __u32 egress_ifindex;   /* txq->dev->ifindex */
};

int bpf_main(void* ctx, uint64_t size);

uint64_t _bpf_helper_ext_0006(const char *fmt, ...);

#endif //BAREMETAL_RV_BPF_HELPERS_H
