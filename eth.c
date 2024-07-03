#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netpacket/packet.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

static int get_interface_index(int sock);
static void get_my_mac_address(int sock, uint8_t mac[]);

int main(void)
{
    // PF_PACKET底层帧
    int sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock < 0)
    {
        printf("creat socket error\n");
        return 1;
    }
    struct sockaddr_ll device = {0};
    device.sll_family = AF_PACKET;
    device.sll_halen = htons(6);
    device.sll_protocol = htons(ETH_P_ALL);
    device.sll_ifindex = get_interface_index(sock);
    // memcpy(device.sll_addr, dst_mac, 6);
    uint8_t data[60] = {0xb0, 0x25, 0xaa, 0x46, 0x37, 0x4e};
    get_my_mac_address(sock, &data[6]);
    data[12] = 9;
    // ll 链路层
    int result = sendto(sock, data, sizeof(data), 0, (struct sockaddr *)&device, sizeof(struct sockaddr_ll));
    if (result == -1)
    {
        printf("send error\n");
        return 1;
    }
    close(sock);
}

static int get_interface_index(int sock)
{
    struct ifreq req;
    strcpy(req.ifr_name, "eth0");
    ioctl(sock, SIOCGIFINDEX, &req);
    return req.ifr_ifindex;
}

static void get_my_mac_address(int sock, uint8_t mac[])
{
    struct ifreq req;
    strcpy(req.ifr_name, "eth0");
    ioctl(sock, SIOCGIFHWADDR, &req);
    memcpy(mac, req.ifr_ifru.ifru_hwaddr.sa_data, 6);
}
