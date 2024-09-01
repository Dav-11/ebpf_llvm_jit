#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_endian.h>

#define ETH_P_ARP 0x0806

struct data_t {
    void *data_end;
    void *data;
    void *pos;
};

struct ethhdr *get_ethhdr_arp(struct data_t *data) {

    if (data->pos + sizeof(struct ethhdr) > data->data_end) {
        return NULL;
    }

    struct ethhdr *eth = data->pos;

    // Increment position to next header
    data->pos += sizeof(struct ethhdr);

    return eth;
}

SEC("xdp")
int hello_world(struct xdp_md *ctx) {

    bpf_printk("Hello World from");

    struct data_t data;
    data.data = (void *)(long)ctx->data;
    data.data_end = (void *)(long)ctx->data_end;
    data.pos = data.data;

    struct ethhdr *eth = get_ethhdr_arp(&data);
    if (!eth) {
        return XDP_DROP;
    }

    if (bpf_ntohs(eth->h_proto) != ETH_P_ARP) {
        return XDP_DROP;
    }


    return XDP_PASS;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";