#include <ft_ping.h>


int main(int c, char **v) {
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
	if (socket < 0)
		printf("error creating the socket\n");


	struct addrinfo *dest;
	struct addrinfo hint;

	memset(&hint, 0, sizeof(hint));

	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_flags = AI_PASSIVE;


	int i = getaddrinfo("google.com", NULL, &hint, &dest);
	if (i) {
		printf("host unreachable\n");
		exit(1);
	}
	
	char *ip = inet_ntoa(((struct sockaddr_in*)dest->ai_addr)->sin_addr);
	printf("google.com: %s\n", ip);
}
