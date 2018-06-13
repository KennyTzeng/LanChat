#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>

#define DEST_MAC0	0xff
#define DEST_MAC1	0xff
#define DEST_MAC2	0xff
#define DEST_MAC3	0xff
#define DEST_MAC4	0xff
#define DEST_MAC5	0xff

#define BUF_SIZE	1024
#define ETH_MY_TYPE	0x0801

int sendPacket(int index, char mac_addr[], char *message) {

	int sockfd;
	int packet_len = 0;
	char sendBuf[BUF_SIZE];
	struct ether_header *eh = (struct ether_header *) sendBuf;
	struct sockaddr_ll socket_address;
	
	/* Open RAW socket to send on */
	if((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
		perror("sender socket");
		return -1;	
	}

	/* Construct the Ethernet header */
	memset(sendBuf, 0, BUF_SIZE);
	/* Ethernet header */
	eh->ether_shost[0] = mac_addr[0];
	eh->ether_shost[1] = mac_addr[1];
	eh->ether_shost[2] = mac_addr[2];
	eh->ether_shost[3] = mac_addr[3];
	eh->ether_shost[4] = mac_addr[4];
	eh->ether_shost[5] = mac_addr[5];
	eh->ether_dhost[0] = DEST_MAC0;
	eh->ether_dhost[1] = DEST_MAC1;
	eh->ether_dhost[2] = DEST_MAC2;
	eh->ether_dhost[3] = DEST_MAC3;
	eh->ether_dhost[4] = DEST_MAC4;
	eh->ether_dhost[5] = DEST_MAC5;
	/* Ethertype field */
	eh->ether_type = htons(ETH_MY_TYPE);
	packet_len += sizeof(struct ether_header);

	/* Packet data */
	strncpy(sendBuf + packet_len, message, strlen(message));
	packet_len += strlen(message);

	/* Index of the network device */
	socket_address.sll_ifindex = index;
	/* Address length*/
	socket_address.sll_halen = ETH_ALEN;
	/* Destination MAC */
	socket_address.sll_addr[0] = DEST_MAC0;
	socket_address.sll_addr[1] = DEST_MAC1;
	socket_address.sll_addr[2] = DEST_MAC2;
	socket_address.sll_addr[3] = DEST_MAC3;
	socket_address.sll_addr[4] = DEST_MAC4;
	socket_address.sll_addr[5] = DEST_MAC5;

	/* Send packet */
	if(sendto(sockfd, sendBuf, packet_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0) {
	    printf("Send failed\n");
		return -1;
	} else {
		// printf("send success. %d %02x:%02x:%02x:%02x:%02x:%02x %s\n", index, (unsigned char) mac_addr[0], (unsigned char) mac_addr[1], (unsigned char) mac_addr[2], (unsigned char) mac_addr[3], (unsigned char) mac_addr[4], (unsigned char) mac_addr[5], message);
	}

	return 0;

}
