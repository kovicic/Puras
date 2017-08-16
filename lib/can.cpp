// --------------------------------------------------------------------------

// local headers
#include <stdio.h>        // printf(), perror()
#include <stdint.h>       // uint32_t
#include <stdlib.h>       // exit()
#include <string.h>       // strcpy()
#include <errno.h>        // errno

#include <net/if.h>       // struct ifreq
#include <sys/ioctl.h>    // ioctl(), SIOCGIFINDEX
#include <linux/can.h>    // PF_CAN, CAN_RAW, struct sockaddr_can

int can_open(const char *interface)
{
	// check Null pointer
	if (!interface) {
		errno = EINVAL;
		perror("can_open");
		exit(-1);
	}

	// Create generic CAN-Bus socket
	int fd_can;
	fd_can = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (fd_can < 0) {
		perror("can_open.socket");
		exit(-1);
	}

	// initialize ifreq (Interface-Request) data structure
	struct ifreq ifreq;
	strcpy(ifreq.ifr_name, interface);

	// search for our interface using ioctrl
	int ret;
	ret = ioctl(fd_can, SIOCGIFINDEX, &ifreq);
	if (ret) {
		perror("can_open.ioctl");
		exit(-1);
	}
	// if succes, then ifreq.ifr_ifindex is filled

	// initialize sockaddr_can data structure
	struct sockaddr_can addr;
	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifreq.ifr_ifindex;
	addr.can_addr.tp = { 0, 0 };

	// connect socket with the interface
	ret = bind(fd_can, (struct sockaddr *)&addr, sizeof(addr));
	if (ret) {
		perror("can_open.bind");
		exit(-1);
	}
	// if success, we can access the interface using read() und write()

	// return file descriptor
	return fd_can;
}
