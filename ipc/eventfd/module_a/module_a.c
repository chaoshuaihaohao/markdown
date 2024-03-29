#include <stdio.h>
#include <string.h>
#include <sys/eventfd.h>
#include <inttypes.h>
#include <fcntl.h>
#include <unistd.h>
//#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <time.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <signal.h>

#include "../common.h"

static int tun_alloc(char *dev)
{
	struct ifreq ifr;
	int fd, err;

	if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
		return -1;

	memset(&ifr, 0, sizeof(ifr));

	/* Flags: IFF_TUN   - TUN device (no Ethernet headers)
	 *        IFF_TAP   - TAP device
	 *
	 *        IFF_NO_PI - Do not provide packet information
	 */
	ifr.ifr_flags = IFF_TAP | IFF_ATTACH_QUEUE;
	if (*dev)
		//strscpy_pad(ifr.ifr_name, dev, IFNAMSIZ);
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);

	if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
		close(fd);
		return err;
	}
//    strcpy(dev, ifr.ifr_name);
	return fd;
}

static void signal_handler(int signum)
{
	printf("捕获到信号:%d\n", signum);

	exit(signum);
}

int main()
{

	signal(SIGINT, signal_handler);
	int efd;
	uint64_t u;
	ssize_t s;
	int ret = 0;

	efd = eventfd(0, 0);
	if (efd == -1) {
		printf("Error init eventfd\n");
		return -1;
	}

	/* Register eventfd to B module */
	int vhost_fd = open("/dev/vhost_test", 0664);
	if (vhost_fd < 0) {
		printf("Error open /dev/vhost_test.\n");
		return vhost_fd;
	}

	/* Open tap device */
	int tap_fd = tun_alloc("hao20");
	if (tap_fd < 0) {
		printf("Error open /dev/net/tun.\n");
		ret = tap_fd;
		goto out;
	}

	ret = ioctl(vhost_fd, B_SET_TUN_FD, &tap_fd);
	if (ret < 0) {
		printf("Error ioctl B_SET_TUN_FD.\n");
		goto out;
	}
#if 1
	ret = ioctl(vhost_fd, B_SET_KICKFD, &efd);
	if (ret < 0) {
		printf("Error ioctl B_SET_KICKFD.\n");
		goto out;
	}
#endif
	/* Register eventfd to module C */
	int kfd;
	kfd = efd;
	int kvm_fd = open("/dev/kvm_test", 0664);
	if (kvm_fd < 0) {
		printf("Error open /dev/kvm\n");
		ret = kvm_fd;
		goto out;
	}

	ret = ioctl(kvm_fd, C_SET_KICKFD, &kfd);
	if (ret < 0) {
		printf("Error ioctl C_SET_KICKFD.\n");
		goto out;
	}
#if 0
	/* no need to read */
	printf("About to read\n");
	s = read(efd, &u, sizeof(uint64_t));
	if (s != sizeof(uint64_t)) {
		printf("Error read eventfd\n");
		return -1;
	}
	printf("Parent read %" PRIu64 " (%#" PRIx64 ") from efd\n", u, u);
#endif
	/* Send random number to module c */
	srand(time(NULL));
	while (1) {
		// 生成0到15之间的随机数  
		//int random_number = rand() % 16;  
		// 生成0到10之间的随机数  
		int random_number = rand() % 11;

		// 打印随机数  
		printf("Sending random number: %d\n", random_number);
		ret = ioctl(kvm_fd, C_NOTIFY, &random_number);
		if (ret < 0) {
			printf("Error ioctl C_NOTIFY.\n");
			return ret;
		}
		// 等待1秒  
		sleep(1);
	}

out:
	close(efd);
	close(tap_fd);
	close(kvm_fd);
	close(vhost_fd);

	return ret;
}
