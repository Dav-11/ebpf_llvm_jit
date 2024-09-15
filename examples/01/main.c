//
// Created by root on 8/27/24.
//

//#include "../../src/include/helpers_impl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

int bpf_main(void* ctx, uint64_t size);

/* user accessible metadata for XDP packet hook
 * new fields must be added to the end of this structure
 */
struct xdp_md {
    __u32 data;
    __u32 data_end;
    __u32 data_meta;
    __u32 ingress_ifindex;  /* rxq->dev->ifindex */
    __u32 rx_queue_index;   /* rxq->queue_index  */
    __u32 egress_ifindex;   /* txq->dev->ifindex */
};

// Function to hexdump the content of the xdp_md structure with char representation
void hexdump_xdp_md(struct xdp_md *xdp) {
    unsigned char *ptr = (unsigned char *)xdp;
    size_t size = sizeof(struct xdp_md);

    printf("Hexdump of xdp_md structure:\n");

    for (size_t i = 0; i < size; i += 16) {
        // Print offset
        printf("%04zx: ", i);

        // Print hex bytes
        for (size_t j = 0; j < 16; j++) {
            if (i + j < size) {
                printf("%02x ", ptr[i + j]);
            } else {
                printf("   "); // padding for the last line if less than 16 bytes
            }
        }

        // Print character representation
        printf(" |");
        for (size_t j = 0; j < 16; j++) {
            if (i + j < size) {
                unsigned char c = ptr[i + j];
                if (isprint(c)) {
                    printf("%c", c);
                } else {
                    printf(".");
                }
            } else {
                printf(" ");
            }
        }
        printf("|\n");
    }
}

// Function to process each received packet
struct xdp_md *create_xdp_struct(unsigned char *buffer, int buflen, int src_ifindex) {

    struct xdp_md *xdp = (struct xdp_md *)malloc(sizeof(struct xdp_md));
    if (xdp == NULL) {
        perror("Malloc Error");
        return NULL;
    }

    // Simulating the filling of xdp_md struct
    xdp->data = (unsigned long)buffer;
    xdp->data_end = (unsigned long)(buffer + buflen);
    xdp->data_meta = 0; // Not using metadata here

    // Ingress interface and queue index would normally come from the NIC,
    // here we mock these values
    xdp->ingress_ifindex = src_ifindex;  // This would be populated based on the NIC
    xdp->rx_queue_index = 0;   // Assuming single queue

    // Since it's userspace, egress_ifindex isn't relevant, set to 0
    xdp->egress_ifindex = 0;

    // Print the data pointers for demonstration
//    printf("Packet processed:\n");
//    printf("  Data: %u\n", xdp->data);
//    printf("  Data End: %u\n", xdp->data_end);
//    printf("  Data Length: %u\n", xdp->data_end - xdp->data);

    return xdp;
}

uint64_t _bpf_helper_ext_0006(uint64_t fmt, uint64_t fmt_size, ...)
{
    const char *fmt_str = (const char *)fmt;
    va_list args;
    va_start(args, fmt_str);

    long ret = vprintf(fmt_str, args);
    va_end(args);
    return 0;
}

void print_addresses(unsigned char *buffer)
{
    // Extract Ethernet header (first 14 bytes)
    struct ether_header *eth = (struct ether_header *)buffer;

    // Check if the packet is IP (ETH_P_IP for IPv4)
    if (ntohs(eth->ether_type) == ETHERTYPE_IP) {
        // Move the pointer to the start of the IP header (after the Ethernet header)
        struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ether_header));

        // Get the source IP address from the IP header
        struct in_addr src_addr, dst_addr;
        src_addr.s_addr = ip->saddr;
        dst_addr.s_addr = ip->daddr;

        // Convert the source IP address to a readable format and print it
//        printf("[SRC: %s", inet_ntoa(src_addr));
//        printf("DST: %s]\n", inet_ntoa(dst_addr));
    } else {
        printf("Not an IP packet\n");
    }
}

int main(int argc, char **argv) {

    int sockfd, err;
    struct ifreq ifr;
    char *ifname;

    // Check if the interface name is provided as a command-line argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <interface-name>\n", argv[0]);
        err = -1;
        goto return_err;
    }

    ifname = argv[1];

    // Get the interface index of the desired interface
    int ifindex = if_nametoindex(ifname);
    if (ifindex == 0) {
        perror("if_nametoindex");
        err = -1;
        goto return_err;
    }

    printf("IFINDEX: %d\n", ifindex);

    unsigned char *buffer = (unsigned char *)malloc(65536);

    struct sockaddr saddr;
    int saddr_len = sizeof(saddr);

    struct xdp_md *xdp;

    // Create a raw socket
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        perror("Socket Error");
        err = 1;
        goto return_err;
    }

    // Set the network interface to promiscuous mode using the interface index
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_ifindex = ifindex;

    // Populate ifr_name using the ifr_ifindex
    if (ioctl(sockfd, SIOCGIFNAME, &ifr) == -1) {
        perror("Error getting interface name");
        err = 1;
        goto close_sock;
    }

    // Now use the ifr_name to get the flags
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) == -1) {
        perror("Error getting interface flags");
        err = 1;
        goto close_sock;
    }

    // Set the interface to promiscuous mode
    ifr.ifr_flags |= IFF_PROMISC;
    if (ioctl(sockfd, SIOCSIFFLAGS, &ifr) == -1) {
        perror("Error setting promiscuous mode");
        err = 1;
        goto close_sock;
    }

    // Listen and process packets
    printf("Start listening...\n");
    while (1) {

        //printf("\n\n ====================================\n");

        int buflen = recvfrom(sockfd, buffer, 65536, 0, &saddr, (socklen_t *)&saddr_len);
        if (buflen < 0) {
            perror("Recvfrom Error");
            break;
        }

        // Print source IP address
        print_addresses(buffer);

        // Create xdp struct
        xdp = create_xdp_struct(buffer, buflen, ifindex);
        //printf("created xdp structure from sock buffer\n");

        int res = bpf_main(&xdp, sizeof(xdp));

        printf("RES: %d\n", res);

        if (res == 2) {
            // printf("XDP_PASS\n");
        } else if (res == 1) {
            printf("\n\n ====================================\n");
            printf("XDP_DROP\n");
            hexdump_xdp_md(xdp);
        }


        free(xdp);
    }

    // Clean up
    close(sockfd);
    free(buffer);
    return 0;

close_sock:
    close(sockfd);

return_err:
    return err;
}

