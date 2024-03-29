#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <sys/ioctl.h>  
#include <linux/if.h>  
#include <linux/if_tun.h>  
  
#define TAP_DEVICE "tap0"  
#define BUFFER_SIZE 2048  
  
int main() {  
    struct ifreq ifr;  
    int fd, n;  
    char buffer[BUFFER_SIZE];  
  
    // 打开 /dev/net/tun  
    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {  
        perror("Opening /dev/net/tun");  
        exit(EXIT_FAILURE);  
    }  
  
    // 设置TAP设备的标志  
    memset(&ifr, 0, sizeof(ifr));  
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI; // IFF_NO_PI 表示不包含额外的包信息头  
  
    // 设置TAP设备的名称  
    strncpy(ifr.ifr_name, TAP_DEVICE, IFNAMSIZ);  
  
    // 调用ioctl创建TAP设备  
    if (ioctl(fd, TUNSETIFF, (void *)&ifr) < 0) {  
        perror("ioctl(TUNSETIFF)");  
        close(fd);  
        exit(EXIT_FAILURE);  
    }  
  
    // 打印TAP设备名称  
    printf("TAP device %s created\n", ifr.ifr_name);  
  
    // 从TAP设备读取数据并输出到标准输出  
    printf("Reading from TAP device %s...\n", ifr.ifr_name);  
    while (1) {  
        n = read(fd, buffer, BUFFER_SIZE);  
        if (n < 0) {  
            perror("Reading from TAP device");  
            break;  
        } else if (n == 0) {  
            printf("End of file on TAP device\n");  
            break;  
        } else {  
            fwrite(buffer, 1, n, stdout);  
        }  
    }  
  
    // 关闭TAP设备  
    close(fd);  
  
    return 0;  
}
