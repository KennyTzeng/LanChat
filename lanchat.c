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

struct InterfaceInfo getNetworkInterfaces();
void lanChat(struct InterfaceInfo);
int sendPacket(int index, char mac_addr[], char *message);
void recvPacket();

struct InterfaceInfo {
	int num;
	int index[32];
	char mac_addr[32][7];
};

int main(int argc, char *argv[]) {
	
	struct InterfaceInfo ifInfo = getNetworkInterfaces();
	lanChat(ifInfo);

	return 0;
}

void lanChat(struct InterfaceInfo ifInfo) {
	
	char name[32], message[128];
	pid_t pid;
	int c, input_pos = 0;
	

	printf("Enter your name: ");
	while(1) {
		c = getchar();
		if(c == EOF || c == '\n'|| input_pos >= 31) {
			name[input_pos] = '\0';
			break;
		}else {
			name[input_pos] = c;
		}
		input_pos++;
	}
	printf("Welcome, '%s'!\n", name);

	for(int i=0; i<ifInfo.num; i++) {
		if((pid = fork()) < 0) {
			perror("fork");
			exit(EXIT_FAILURE);
		} else if(pid == 0) {
			// recvPacket();
			exit(0);
		}	
	}

	while(1) {
		memset(message, 0, 128);
		input_pos = 0;
		
		while(1) {
			c = getchar();
			if(c == EOF || c == '\n' || input_pos >= 127) {
				message[input_pos] = '\0';
				break;
			}else {
				message[input_pos] = c;
			}
			input_pos++;
		}

		for(int j=0; j<ifInfo.num; j++) {
			sendPacket(ifInfo.index[j], ifInfo.mac_addr[j], message);
		}
	}

	return;

}

struct InterfaceInfo getNetworkInterfaces() {
	
	struct InterfaceInfo ifInfo;
	struct ifaddrs *ifaddr, *ifa;
	struct ifreq ifr;
	int interfaceIndex;
	char addr[16], mask[16], broadaddr[16];

	memset(&ifInfo, 0, sizeof(struct InterfaceInfo));

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

			ifInfo.index[ifInfo.num] = interfaceIndex;
			ifInfo.mac_addr[ifInfo.num][0] = ifr.ifr_addr.sa_data[0];
			ifInfo.mac_addr[ifInfo.num][1] = ifr.ifr_addr.sa_data[1];
			ifInfo.mac_addr[ifInfo.num][2] = ifr.ifr_addr.sa_data[2];
			ifInfo.mac_addr[ifInfo.num][3] = ifr.ifr_addr.sa_data[3];
			ifInfo.mac_addr[ifInfo.num][4] = ifr.ifr_addr.sa_data[4];
			ifInfo.mac_addr[ifInfo.num][5] = ifr.ifr_addr.sa_data[5];
			ifInfo.num = ifInfo.num + 1;			

			close(fd);
			
		}

	}

	freeifaddrs(ifaddr);


	return ifInfo;
}

