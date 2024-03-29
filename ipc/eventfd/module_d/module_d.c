#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <fcntl.h>
#include <stdint.h>


int main()
{
	int s;
	uint64_t u;
	int fd = open("/dev/net/tun", O_RDWR, 0);
	if (fd < 0) {
		printf("Error open /dev/tun\n");
	}

	printf("about to read\n");
	s = read(fd, &u, sizeof(uint64_t));
	if (s != sizeof(uint64_t))
		printf("read error\n");
	printf("read %"PRIu64" (%#"PRIx64") from efd\n", u, u);

}
