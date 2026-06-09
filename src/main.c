#include <ft_ping.h>


int main(int c, char **v) {
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
	if (socket < 0)
		printf("error creating the socket\n");
}
