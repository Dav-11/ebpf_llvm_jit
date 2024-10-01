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

SEC("xdp")
int hello_world(struct xdp_md *ctx) {

    int ret = 0;
    ret += 7;

    if(ret > 6) {
      ret -=6;
    }

    return ret;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";
