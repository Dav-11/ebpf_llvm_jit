//
// Created by root on 8/27/24.
//

#include "../../src/include/helpers_impl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
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

// Function to process each received packet
struct xdp_md *create_xdp_struct(unsigned char *buffer, int buflen) {

    struct xdp_md *xdp = malloc(sizeof(struct xdp_md));

    // Simulating the filling of xdp_md struct
    xdp->data = (unsigned long)buffer;
    xdp->data_end = (unsigned long)(buffer + buflen);
    xdp->data_meta = 0; // Not using metadata here

    // Ingress interface and queue index would normally come from the NIC,
    // here we mock these values
    xdp->ingress_ifindex = 0;  // This would be populated based on the NIC
    xdp->rx_queue_index = 0;   // Assuming single queue

    // Since it's userspace, egress_ifindex isn't relevant, set to 0
    xdp->egress_ifindex = 0;

    // Print the data pointers for demonstration
    printf("Packet processed:\n");
    printf("  Data: %p\n", (void *)xdp->data);
    printf("  Data End: %p\n", (void *)xdp->data_end);
    printf("  Data Length: %ld\n", xdp->data_end - xdp->data);

    return xdp;
}

int main() {

    int sockfd, err;
    struct ifreq ifr;
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

    // Set the network interface to promiscuous mode
    strncpy(ifr.ifr_name, "ens160", IFNAMSIZ-1);
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) == -1) {
        perror("Error getting interface flags");
        err = 1;
        goto close_sock;
    }
    ifr.ifr_flags |= IFF_PROMISC;
    if (ioctl(sockfd, SIOCSIFFLAGS, &ifr) == -1) {
        perror("Error setting promiscuous mode");
        err = 1;
        goto close_sock;
    }

    // Listen and process packets
    printf("Start listening...\n");
    while (1) {
        int buflen = recvfrom(sockfd, buffer, 65536, 0, &saddr, (socklen_t *)&saddr_len);
        if (buflen < 0) {
            perror("Recvfrom Error");
            break;
        }

        // Create xdp struct
        xdp = create_xdp_struct(buffer, buflen);
        bpf_main(&xdp, sizeof(xdp));

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
