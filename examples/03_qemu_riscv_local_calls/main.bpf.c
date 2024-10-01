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

static int __attribute__((noinline)) test_02() {

  int ret = 99;

    if (ret >8) {
        ret -= 8;
    }

    return ret;
}

static int __noinline test_01(struct data_t *data, struct ethhdr *eth) {

  int ret = 0;

  ret += bpf_htons(ETH_P_IP);

    return ret;
}

SEC("xdp")
int hello_world(struct xdp_md *ctx) {

    struct data_t data;
    struct ethhdr eth;
    int h_proto;

    data.data = (void *)(long)ctx->data;
    data.data_end = (void *)(long)ctx->data_end;
    data.pos = data.data;

    h_proto = test_01(&data, &eth);
    if (h_proto < 0) {
        return 1;
    }

    if (h_proto != bpf_htons(ETH_P_IP)) {
        return 2;
    }

    return test_02();
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";
