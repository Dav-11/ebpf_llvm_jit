#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <stddef.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <bpf/bpf_endian.h>
#include <stdint.h>

struct data_t {
    void *data_end;
    void *data;
    void *pos;
};

static __always_inline int parse_ethhdr(struct data_t *data, struct ethhdr *eth) {

    if (data->pos + sizeof(struct ethhdr) > data->data_end) {
        return -1;
    }

    eth = (struct ethhdr *)data->pos;
    data->pos += sizeof(struct ethhdr);

    return eth->h_proto;
}

SEC("xdp")
int hello_world(struct xdp_md *ctx) {

    bpf_printk("Hello World from");

    struct data_t data;
    struct ethhdr eth;
    int h_proto;

    data.data = (void *)(long)ctx->data;
    data.data_end = (void *)(long)ctx->data_end;
    data.pos = data.data;

    h_proto = parse_ethhdr(&data, &eth);
    if (h_proto < 0) {
        return XDP_DROP;
    }

    if (eth.h_proto != bpf_htons(ETH_P_IP)) {
        return XDP_PASS;
    }

    return XDP_PASS;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";
