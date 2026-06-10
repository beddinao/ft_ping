#include <ft_ping.h>

_data	g_vars;

unsigned short csum(unsigned short *buff, int words_n) {
	unsigned short sum = 0;
	while (words_n--)
		sum += *buff++;
	sum = (sum >> 16) + (sum & 0xffff);
	return ~sum;
}

bool op_general_check(bool op_var) { return (op_var || g_vars.input.version || g_vars.input.help); }

bool op_version(int c, char **v, int *current_arg) {
	printf("ft_ping: %s\n", ft_ping_version);
	return False;
}

bool op_help(int c, char **v, int *current_arg) {
	return False;
}

bool op_verbose(int c, char **v, int *current_arg) {
	printf("verbose detected!\n");
	if (op_general_check(g_vars.input.verbose)) return False;
	g_vars.input.verbose = True;
	return True;
}

bool op_flood(int c, char **v, int *current_arg) {
	printf("flood detected!\n");
	if (op_general_check(g_vars.input.flood)) return False;
	g_vars.input.flood = True;
	return True;
}

bool op_numeric(int c, char **v, int *current_arg) {
	printf("numeric detected!\n");
	if (op_general_check(g_vars.input.numeric_only)) return False;
	g_vars.input.numeric_only = True;
	return True;
}

bool op_count(int c, char **v, int *current_arg) {
	printf("count detected!\n");
	if (op_general_check(g_vars.input.is_set_count)) return False;
	g_vars.input.is_set_count = True;
	return True;
}

bool op_interval(int c, char **v, int *current_arg) {
	printf("interval detected!\n");
	if (op_general_check(g_vars.input.is_set_interval)) return False;
	g_vars.input.is_set_interval = True;
	return True;
}

bool op_timeout(int c, char **v, int *current_arg) {
	printf("timeout detected!\n");
	if (op_general_check(g_vars.input.is_set_timeout)) return False;
	g_vars.input.is_set_timeout = True;
	return True;
}

bool op_tos(int c, char **v, int *current_arg) {
	printf("tos detected!\n");
	if (op_general_check(g_vars.input.is_set_tos)) return False;
	g_vars.input.is_set_tos = True;
	return True;
}

bool op_ttl(int c, char **v, int *current_arg) {
	printf("ttl detected!\n");
	if (op_general_check(g_vars.input.is_set_ttl)) return False;
	g_vars.input.is_set_ttl = True;
	return True;
}

bool parse_params(int c, char **v) {
	int	current_arg;
	bool	valid_arg;
	uint8_t	current_possible_param;
	char	possible_one_char_param[0x3];
	char	*one_char_args = "-V-?-v-f-n-c-i-W-Q-t";
	char	*full_name_args[0xa] = {
		"--version", "--help", "--verbose", "--flood", "--numeric",
		"--count", "--interval", "--timeout", "--tos", "--ttl"
	};
	bool	(*param_ops[0xa])(int, char**, int*) = {
		op_version, op_help, op_verbose, op_flood, op_numeric,
		op_count, op_interval, op_timeout, op_tos, op_ttl
	};
	

	current_arg = 1;
	while (current_arg < c - 1) {
		valid_arg = False;
		current_possible_param = 0;
		while (current_possible_param < 0xa) {
			possible_one_char_param[0] = one_char_args[current_possible_param*2];
			possible_one_char_param[1] = one_char_args[current_possible_param*2+1];
			possible_one_char_param[2] = '\0';
			if (!strcmp(v[current_arg], full_name_args[current_possible_param])
				|| !strcmp(v[current_arg], possible_one_char_param)) {
				valid_arg = param_ops[current_possible_param](c, v, &current_arg);
				break;
				
			}
			current_possible_param += 1;
		}
		if (!valid_arg)
			return False;
		current_arg += 1;
	}

	return True;
}

int main(int c, char **v) {
	printf("c: %i\n", c);
	if (c < 2 || (c > 2 && !parse_params(c, v))) {
		//display_help();
		printf("help page\n");
		exit(1);
	}

	printf("goin for [%s]\n", v[c-1]);
	//print_params();
}

int a(int c, char **v) {
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

	char packet[4000], sec_packet[4000];
	int sequence = 0;

	fd_set r_set;
	struct timeval tv = {3, 0};
	
	struct icmphdr in_icmphdr;

	socklen_t addr_size = sizeof(struct sockaddr);

	while (1) {
		struct icmphdr *icmp_hdr = (struct icmphdr*)packet;
		memset(packet, 0, sizeof(packet));
		icmp_hdr->type = ICMP_ECHO;
		icmp_hdr->un.echo.id = 2003;
		icmp_hdr->un.echo.sequence = sequence++;
		icmp_hdr->checksum = csum((unsigned short*)packet, sizeof(struct icmphdr)/2);

		i = sendto(sock, packet, sizeof(struct icmphdr), 0, dest->ai_addr, sizeof(struct sockaddr));

		if (i < 0) {
			perror("sendto");
			break;
		}
		
		FD_ZERO(&r_set);
		FD_SET(sock, &r_set);
		//usleep(250000);
		i = select(sock + 1, &r_set, NULL, NULL, &tv);
		if (i < 0) {
			perror("select");
			break;
		}
		if (!i) {
			printf("time out\n");
			break;
		}
		memset(packet, 0, sizeof(packet));
		int j = 0;
		while (j < sizeof(struct icmphdr)) {
			memset(sec_packet, 0, sizeof(sec_packet));
			i = recvfrom(sock, sec_packet, sizeof(sec_packet), 0, dest->ai_addr, &addr_size);
			if (i < 0) {
				perror("recvfrom");
				exit(1);
			}
			memcpy(packet + j, sec_packet, i);
			j += i;
		}
		memset(&in_icmphdr, 0, sizeof(struct icmphdr));
		memcpy(&in_icmphdr, packet, sizeof(struct icmphdr));
		printf("echo.id=%i, icmp_seq=%i\n", in_icmphdr.un.echo.id, in_icmphdr.un.echo.sequence);

	}
}
