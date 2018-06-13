
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

#define BUF_SIZE	1024
#define ETH_MY_TYPE	0x0801

int recvPacket(char *ifName) {

	int sockfd;
	int sockopt;
	ssize_t numBytes;
	struct ifreq ifopts;	/* set promiscuous mode */
	uint8_t recvBuf[BUF_SIZE];

	/* Open PF_PACKET socket, listening for EtherType ETH_MY_TYPE */
	if((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_MY_TYPE))) == -1) {
		perror("listener socket");	
		return -1;
	}

	/* Set interface to promiscuous mode - do we need to do this every time? */
	/*
	strncpy(ifopts.ifr_name, ifName, IFNAMSIZ-1);
	ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
	ifopts.ifr_flags |= IFF_PROMISC;
	ioctl(sockfd, SIOCSIFFLAGS, &ifopts);
	*/
	/* Allow the socket to be reused - incase connection is closed prematurely */
	/*
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) == -1) {
		perror("set sockopt");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	*/
	
	/* Bind to device */
	if(setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, ifName, IFNAMSIZ-1) == -1)	{
		perror("SO_BINDTODEVICE");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	while(1) {

		// printf("listener: Waiting to recvfrom...\n");
		numBytes = recvfrom(sockfd, recvBuf, BUF_SIZE, 0, NULL, NULL);
		// printf("listener: got packet %lu bytes\n", numBytes);

		/* Print packet */
		printf(">>> <%02x:%02x:%02x:%02x:%02x:%02x> ", 
			(unsigned char)recvBuf[6],
			(unsigned char)recvBuf[7],
			(unsigned char)recvBuf[8],
			(unsigned char)recvBuf[9],
			(unsigned char)recvBuf[10],
			(unsigned char)recvBuf[11]);
	
		for(int i = 14; i < numBytes; i++) {
			printf("%c", recvBuf[i]);
		}
		printf("\n");

	}

	close(sockfd);
	return 0;
}
