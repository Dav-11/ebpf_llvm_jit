#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <stddef.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <bpf/bpf_endian.h>
#include <stdint.h>
#include <string.h>

SEC("xdp")
int hello_world(struct xdp_md *ctx) {

    bpf_printk("Hello World from XDP: %s, %s\n", "abcdef", "ghilm");
    bpf_printk("Hola Mundo from XDP: %s, %s\n", "aaaaaaa", "kkkkkkkk");
    return XDP_PASS;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";
