#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <linux/if_link.h>
#include <linux/if.h>

void getNetworkInterfaces();

int main(int argc, char *argv[]) {
	
	getNetworkInterfaces();

	return 0;
}

void getNetworkInterfaces() {

	struct ifaddrs *ifaddr, *ifa;
	int family, s, n;
	char host[NI_MAXHOST];
	struct ifreq ifr;

	if(getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}

	for(ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
		if(ifa->ifa_addr == NULL) {
			continue;
		}

		family = ifa->ifa_addr->sa_family;

		printf("%-8s %s (%d)\n", 
			ifa->ifa_name, 
			(family == AF_PACKET)? "AF_PACKET" :
			(family == AF_INET)? "AF_INET" :
			(family == AF_INET6)? "AF_INET6" : "???",
			family);

		if(family == AF_INET || family == AF_INET6) {
			s = getnameinfo(ifa->ifa_addr, (family == AF_INET)? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if(s != 0) {
				printf("getnameinfo() failed: %s\n", gai_strerror(s));
				exit(EXIT_FAILURE);
			}

			printf("\t\taddress: %s\n", host);

			int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
			strcpy(ifr.ifr_name, ifa->ifa_name);
			if(ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) {
				for(int i=0; i<6; i++) {
					printf("%02x", (unsigned char) ifr.ifr_addr.sa_data[i]);
				}
				puts("\n");
			}
			close(fd);

		}else if(family == AF_PACKET && ifa->ifa_data != NULL) {
			struct rtnl_link_stats *stats = ifa->ifa_data;

			printf("\t\ttx_packets = %10u; rx_packets = %10u\n"
				   "\t\ttx_bytes   = %10u; rx_bytes   = %10u\n",
				   stats->tx_packets, stats->rx_packets,
				   stats->tx_bytes, stats->rx_bytes);
			

			printf("%s\n", ifa->ifa_data);
		}
	}

	freeifaddrs(ifaddr);


	return;
}