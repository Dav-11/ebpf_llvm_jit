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

//static __always_inline int parse_ethhdr(struct data_t *data, struct ethhdr *eth) {
//
//    eth = (struct ethhdr *)data->pos;
//
//    if (data->pos + sizeof(eth) > data->data_end) {
//        return -1;
//    }
//
//    data->pos += sizeof(eth);
//
//    return eth->h_proto;
//}

SEC("xdp")
int hello_world(struct xdp_md *ctx) {

//    bpf_printk("Hello World from XDP");

//    struct data_t data;
//    struct ethhdr eth;
//    int h_proto;
//
//    data.data = (void *)(long)ctx->data;
//    data.data_end = (void *)(long)ctx->data_end;
//    data.pos = data.data;
//
//    h_proto = parse_ethhdr(&data, &eth);
//    if (h_proto < 0) {
//        return XDP_DROP;
//    }
//
//    bpf_printk("h_proto: %d", h_proto);
//
//    if (h_proto != bpf_htons(ETH_P_IP)) {
//        return XDP_PASS;
//    }

    return XDP_PASS;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";
