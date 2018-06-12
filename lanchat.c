#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <linux/if_link.h>
//#include <linux/if.h>
#include <arpa/inet.h>
#include <net/if.h>

void getNetworkInterfaces();
void lanChat();
void sendPacket();
void recvPacket();

int main(int argc, char *argv[]) {
	
	getNetworkInterfaces();
	lanChat();

	return 0;
}

void getNetworkInterfaces() {

	struct ifaddrs *ifaddr, *ifa;
	struct ifreq ifr;
	int interfaceIndex;
	char addr[16], mask[16], broadaddr[16];

	printf("Enumerated network interfaces:\n");

	if(getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}

	for(ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		
		if(ifa->ifa_addr == NULL) {
			continue;
		}

		if(ifa->ifa_addr->sa_family == AF_INET) {
			
			interfaceIndex = if_nametoindex(ifa->ifa_name);
			strcpy(addr, inet_ntoa(((struct sockaddr_in *)ifa->ifa_addr)->sin_addr));
			strcpy(mask, inet_ntoa(((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr));
			strcpy(broadaddr, inet_ntoa(((struct sockaddr_in *)((ifa->ifa_ifu).ifu_broadaddr))->sin_addr));			
			printf("%-3d - %-10s %-15s %-15s (%-15s) ", interfaceIndex, ifa->ifa_name, addr, mask, broadaddr);
			

			/*
			printf("address: %s\n", inet_ntoa(((struct sockaddr_in *) ifa->ifa_addr)->sin_addr));
			printf("mask: %s\n", inet_ntoa(((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr));
			printf("bmask: %s\n", inet_ntoa(((struct sockaddr_in *)((ifa->ifa_ifu).ifu_broadaddr))->sin_addr));			
			*/

			int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
			strcpy(ifr.ifr_name, ifa->ifa_name);
			if(ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) {
				printf("%02x:%02x:%02x:%02x:%02x:%02x\n", 
					(unsigned char) ifr.ifr_addr.sa_data[0],
					(unsigned char) ifr.ifr_addr.sa_data[1],
					(unsigned char) ifr.ifr_addr.sa_data[2],
					(unsigned char) ifr.ifr_addr.sa_data[3],
					(unsigned char) ifr.ifr_addr.sa_data[4],
					(unsigned char) ifr.ifr_addr.sa_data[5]);
			}
			close(fd);
			
		}

	}

	freeifaddrs(ifaddr);


	return;
}

void lanChat() {
	
	char name[32], message[128];
	pid_t pid;
	
	printf("Enter your name: ");
	scanf("%32s", name);
	printf("Welcome, '%s'!\n", name);

	if((pid = fork()) < 0) {
		perror("fork");
		exit(EXIT_FAILURE);
	} else if(pid == 0) {
		recvPacket();
		exit(0);
	}	
	
	while(1) {
		memset(message, 0, 128);
		scanf("%128s", message);
		sendPacket();
	}

	return;
}

void sendPacket() {

	printf("send\n");	

}

void recvPacket() {

	printf("recv\n");	

}
