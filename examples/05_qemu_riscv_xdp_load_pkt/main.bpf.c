#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <stddef.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <bpf/bpf_endian.h>
#include <stdint.h>
#include <string.h>

#define ETH_P_IP 0x0800

struct data_t {
    void *data_end;
    void *data;
    void *pos;
};

SEC("xdp")
int hello_world(struct xdp_md *ctx) {

    bpf_printk("Hello World from XDP\n");
    bpf_printk("ctx = 0x%16x\n",ctx);

    bpf_printk("ctx->data\t= 0x%8x\n",ctx->data);
    bpf_printk("ctx->data_end\t= 0x%8x\n",ctx->data_end);

    struct data_t data;
    data.data = (void *)(long)ctx->data;
    data.data_end = (void *)(long)ctx->data_end;

    bpf_printk("data.data\t= 0x%16x\n",data.data);
    bpf_printk("data.data_end\t= 0x%16x\n",data.data_end);

    if ((data.data == NULL) || (data.data_end == NULL)) {

        bpf_printk("ctx is NULL\n");
        return XDP_PASS;
    }
    
    data.pos = data.data;
    
    if (data.pos + sizeof(struct ethhdr) > data.data_end) {

        bpf_printk("not big enough for ethhdr\n");
        return XDP_PASS;
    }

    struct ethhdr *eth = data.pos;
    data.pos += sizeof(struct ethhdr);

    bpf_printk("ETH_HDR_BASE\t= 0x%16x\n", eth);
    bpf_printk("ETH_HDR_END\t= 0x%16x\n", data.pos);


    // Check if packet is IP
    int proto = bpf_ntohs(eth->h_proto);
    //int proto = eth->h_proto;

    bpf_printk("eth->h_proto = %x (bpf_ntohs(eth->h_proto) = %x)\n", eth->h_proto, bpf_ntohs(eth->h_proto));
    bpf_printk("ETH_P_IP = %x (%d)\n", ETH_P_IP, ETH_P_IP);

    if ( proto == ETH_P_IP) {

        bpf_printk("Packet is IP\n");
        return XDP_DROP;
    } else {
        bpf_printk("Packet is not IP\n");
    }

    return XDP_PASS;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";
