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

static __always_inline void print_ip(uint32_t *val)
{
    uint8_t *saddr = (uint8_t *) val;
    for (int i = 0; i < 4; i++) {
        bpf_printk("%d", saddr[i]);

        if (i != 3) {
            bpf_printk(".");
        }
    }

    bpf_printk(" (0x%x)\n", *val);
}

static __always_inline int parse_iphdr(void *data, void *data_end, __u16 *nh_off, struct iphdr **iphdr) {
   struct iphdr *ip = (struct iphdr *)(data + *nh_off);
   int hdr_size = sizeof(*ip);

   /* Byte-count bounds check; check if current pointer + size of header
    * is after data_end.
    */
   if ((void *)ip + hdr_size > data_end)
      return -1;

   hdr_size = ip->ihl * 4;
   if (hdr_size < sizeof(*ip))
      return -1;

   /* Variable-length IPv4 header, need to use byte-based arithmetic */
   if ((void *)ip + hdr_size > data_end)
      return -1;

   *nh_off += hdr_size;
   *iphdr = ip;

   return ip->protocol;
}

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

    if ( proto != ETH_P_IP) {

        bpf_printk("Packet is not IP\n");
        return XDP_PASS;
    }

    bpf_printk("Packet is IP\n");

    struct iphdr *ip = (struct iphdr *) data.pos;
    int hdr_size = sizeof(*ip);

    if ((void *)ip + hdr_size > data.data_end) {

        bpf_printk("ip + hdr_size > data_end\n");
        return -1;
    }

    hdr_size = ip->ihl * 4;
    if (hdr_size < sizeof(*ip)) {

        bpf_printk("hdr_size < sizeof(*ip)\n");
        return -2;
    }

    /* Variable-length IPv4 header, need to use byte-based arithmetic */
    if ((void *)ip + hdr_size > data.data_end) {

        bpf_printk("ip + hdr_size > data_end\n");
        return -3;
    }

    bpf_printk("source_address:\t");
    print_ip(&ip->saddr);

    bpf_printk("dest_address:\t");
    print_ip(&ip->daddr);

    return XDP_PASS;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";
