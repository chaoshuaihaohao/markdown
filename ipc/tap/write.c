#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <sys/ioctl.h>  
#include <linux/if.h>  
#include <linux/if_tun.h>  
#include <netinet/ether.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
  
int main() {  
    struct ifreq ifr;  
    int fd, err;  
    const char *tun_name = "tap0";  
    unsigned char buffer[2048];  
    struct ether_header *eh = (struct ether_header *)buffer;  
  
    // 打开 /dev/net/tun  
    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {  
        perror("Opening /dev/net/tun");  
        exit(EXIT_FAILURE);  
    }  
  
    // 设置TAP设备的标志  
    memset(&ifr, 0, sizeof(ifr));  
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI; // IFF_NO_PI 表示不包含额外的包信息头  
  
    // 设置TAP设备的名称  
    if (*tun_name != '\0') {  
        strncpy(ifr.ifr_name, tun_name, IFNAMSIZ);  
    }  
  
    // 调用ioctl创建TAP设备  
    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {  
        perror("ioctl(TUNSETIFF)");  
        close(fd);  
        exit(EXIT_FAILURE);  
    }  
  
    // 打印TAP设备名称  
    printf("TAP device %s created\n", ifr.ifr_name);  

#if 1
	int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    // 调用ioctl up TAP设备 
   ifr.ifr_flags = IFF_UP|IFF_BROADCAST|IFF_RUNNING|IFF_MULTICAST;
    if ((err = ioctl(sockfd, SIOCSIFFLAGS, (void *)&ifr)) < 0) {  
        perror("ioctl(TUNSETIFF)");  
        close(sockfd);  
        exit(EXIT_FAILURE);  
    }  
#endif



    // 构造以太网帧头部  
    memset(eh, 0, sizeof(struct ether_header));  
    // 设置目标MAC地址（这里随便设置了一个地址，通常需要根据实际情况来设定）  
    memcpy(eh->ether_dhost, "\xff\xff\xff\xff\xff\xff", ETHER_ADDR_LEN);  
    // 设置源MAC地址（同样需要根据实际情况来设定）  
    memcpy(eh->ether_shost, "\x00\x00\x00\x00\x00\x01", ETHER_ADDR_LEN);  
    // 设置以太网类型字段为IPv4  
    eh->ether_type = htons(ETH_P_IP);  
  
    // 将数据"hello world"添加到以太网帧的有效负载中  
    strcpy((char *)(eh + 1), "hello world");  
    int packet_size = sizeof(struct ether_header) + strlen("hello world");  
  
    sleep(20);
    // 发送以太网帧到TAP设备  
    if (write(fd, buffer, packet_size) < 0) {  
        perror("Writing to TAP device");  
        close(fd);  
        exit(EXIT_FAILURE);  
    }  
  
    printf("Wrote %d bytes to TAP device\n", packet_size);  
  
    sleep(1000);
    // 关闭TAP设备  
    close(fd);  
    close(sockfd);  
  
    return 0;  
}
