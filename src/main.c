#include <ft_ping.h>

_data	g_vars;

unsigned short csum(unsigned short *buff, int words_n) {
	unsigned short sum = 0;
	while (words_n--)
		sum += *buff++;
	sum = (sum >> 16) + (sum & 0xffff);
	return ~sum;
}

void	display_help() {
	printf("\nUsage\n  ft_ping [options] <destination>\n");
	printf("\nOptions:\n");
	printf("  <destination>             dns name or ip address\n");
	printf("  -c --count <count>        stop after <count> replies\n");
	printf("                              0xffffffff > <count> >= 0\n");
	printf("                              default: infinite\n");
	printf("  -i --interval <interval>  time between sending each packet in seconds\n");
	printf("                              0xffffffff > <interval> >= 0\n");
	printf("                              default: 1\n");
	printf("  -W --timeout <timeout>    time to wait for response in milliseconds\n");
	printf("                              0xffffffff > <timeout> >= 0\n");
	printf("                              default: 10000 == 10s\n");
	printf("  -Q --tos <tos>            set Quality Of Serivce bits\n");
	printf("                              0xff < <tos> >= 0\n");
	printf("                              default: 0\n");
	printf("  -t --ttl <ttl>            set Time To Live <ttl> bits\n");
	printf("                              0xff > <ttl> >= 0\n");	
	printf("                              default: 0x40\n");
	printf("  -f --flood                flood ping\n");
	printf("  -n --numeric              no dns name resolution\n");
	printf("  -v --verbose              verbose output\n");
	printf("  -V --version              prints version and exits\n");
	printf("  -? --help                 shows this help and exits\n\n");
}

bool 	op_general_check(bool op_var) { return (op_var || g_vars.input.version || g_vars.input.help); }

bool 	op_help(int c, char **v, int *current_arg) {
	return False;
}

bool	op_verbose(int c, char **v, int *current_arg) {
	if (op_general_check(g_vars.input.verbose)) return False;
	g_vars.input.verbose = True;
	return True;
}

bool op_flood(int c, char **v, int *current_arg) {
	if (op_general_check(g_vars.input.flood)) return False;
	g_vars.input.flood = True;
	return True;
}

bool op_numeric(int c, char **v, int *current_arg) {
	if (op_general_check(g_vars.input.numeric_only)) return False;
	g_vars.input.numeric_only = True;
	return True;
}

bool op_count(int c, char **v, int *current_arg) {
	if (op_general_check(g_vars.input.is_set_count)
		|| c - (*current_arg + 1) < 2) return False;
	errno = 0;
	(*current_arg)++;
	char	*endptr;
	int64_t	count_nm = strtol(v[*current_arg], &endptr, 0xa);
	if (errno == ERANGE
		|| endptr == v[*current_arg] || *endptr != '\0'	
		|| count_nm < 0 || count_nm >= 0xffffffff) {
		printf("invalid \"Count\" value: %s\n", v[*current_arg]);
		return False;
	}
	g_vars.input.is_set_count = True;
	g_vars.input.count = count_nm;
	return True;
}

bool op_interval(int c, char **v, int *current_arg) {
	if (op_general_check(g_vars.input.is_set_interval)
		|| c - (*current_arg + 1) < 2) return False;
	errno = 0;
	(*current_arg)++;
	char	*endptr;
	int64_t	interval_nm = strtol(v[*current_arg], &endptr, 0xa);
	if (errno == ERANGE
		|| endptr == v[*current_arg] || *endptr != '\0'
		|| interval_nm < 0 || interval_nm >= 0xffffffff) {
		printf("invalid \"Interval\" value: %s\n", v[*current_arg]);
		return False;
	}
	g_vars.input.is_set_interval = True;
	g_vars.input.interval = interval_nm;
	return True;
}

bool op_timeout(int c, char **v, int *current_arg) {
	if (op_general_check(g_vars.input.is_set_timeout)
		|| c - (*current_arg + 1) < 2) return False;
	errno = 0;
	(*current_arg)++;
	char	*endptr;
	int64_t	timeout_nm = strtol(v[*current_arg], &endptr, 0xa);
	if (errno == ERANGE
		|| endptr == v[*current_arg] || *endptr != '\0'
		|| timeout_nm < 0 || timeout_nm >= 0xffffffff) {
		printf("invalid \"Timeout\" value: %s\n", v[*current_arg]);
		return False;
	}
	g_vars.input.is_set_timeout = True;
	g_vars.input.timeout = timeout_nm;
	return True;
}

bool op_tos(int c, char **v, int *current_arg) {
	if (op_general_check(g_vars.input.is_set_tos)
		|| c - (*current_arg + 1) < 2) return False;
	errno = 0;
	(*current_arg)++;
	char	*endptr;
	int64_t	tos_nm = strtol(v[*current_arg], &endptr, 0xa);
	if (errno == ERANGE
		|| endptr == v[*current_arg] || *endptr != '\0'
		|| tos_nm < 0 || tos_nm >= 0xff) {
		printf("invalid \"Quality Of Serice (QOS/TOS)\" value: %s\n", v[*current_arg]);
		return False;
	}
	g_vars.input.is_set_tos = True;
	g_vars.input.tos = tos_nm;
	return True;
}

bool op_ttl(int c, char **v, int *current_arg) {
	if (op_general_check(g_vars.input.is_set_ttl)
		|| c - (*current_arg + 1) < 2) return False;
	errno = 0;
	(*current_arg)++;
	char	*endptr;
	int64_t	ttl_nm = strtol(v[*current_arg], &endptr, 0xa);
	if (errno == ERANGE
		|| endptr == v[*current_arg] || *endptr != '\0'
		|| ttl_nm < 0 || ttl_nm >= 0xff) {
		printf("invalid \"Time To Live (TTL)\" value: %s\n", v[*current_arg]);
		return False;
	}
	g_vars.input.is_set_ttl = True;
	g_vars.input.ttl = ttl_nm;
	return True;
}

bool parse_params(int c, char **v) {
	bool	valid_arg;
	char	possible_one_char_param[0x3];
	char	*one_char_args = "-v-f-n-c-i-W-Q-t";
	char	*full_name_args[0x8] = {
		"--verbose", "--flood", "--numeric",
		"--count", "--interval", "--timeout", "--tos", "--ttl"
	};
	bool	(*param_ops[0x8])(int, char**, int*) = {
		op_verbose, op_flood, op_numeric,
		op_count, op_interval, op_timeout, op_tos, op_ttl
	};


	if (c == 2 && (!strcmp(v[c-1], "-V") || !strcmp(v[c-1], "--version"))) {
		printf("ft_ping: %s\n\n", ft_ping_version);
		exit(False);
	}
	else if (c == 2 && (!strcmp(v[c-1], "-?") || !strcmp(v[c-1], "--help")))
		return False;

	for (int current_arg = 1; current_arg < c - 1; current_arg+=1) {
		valid_arg = False;
		for (uint8_t current_possible_param = 0; current_possible_param < 0xa; current_possible_param += 1) {
			possible_one_char_param[0] = one_char_args[current_possible_param*2];
			possible_one_char_param[1] = one_char_args[current_possible_param*2+1];
			possible_one_char_param[2] = '\0';
			if (!strcmp(v[current_arg], full_name_args[current_possible_param])
				|| !strcmp(v[current_arg], possible_one_char_param)) {
				valid_arg = param_ops[current_possible_param](c, v, &current_arg);
				break;
			}
		}
		if (!valid_arg) 
			return False;
	}

	return True;
}

int main(int c, char **v) {
	if (c < 2 || c > 0xff || (c >= 2 && !parse_params(c, v))) {
		display_help();
		return 1;
	}


	printf("\nBOOLS:\n--version: %i\n--help: %i\n--verbose: %i\n--flood: %i\n--numeric_only: %i\n--is_set_count: %i\n--is_set_internval: %i\n--is_set_timeout: %i\n--is_set_ttl: %i\n--is_set_tos: %i\n\n",
			g_vars.input.version, g_vars.input.help, g_vars.input.verbose, g_vars.input.flood, g_vars.input.numeric_only, g_vars.input.is_set_count, g_vars.input.is_set_interval, g_vars.input.is_set_timeout, g_vars.input.is_set_ttl, g_vars.input.is_set_tos);

	printf("\nVALUES:\n--count: %i\n--interval: %i\n--timeout: %i\n--tos: %i\n--ttl: %i\n\n",
			g_vars.input.count, g_vars.input.interval, g_vars.input.timeout, g_vars.input.tos, g_vars.input.ttl);

	printf("goin for [%s]\n", v[c-1]);
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
	return 1;
}
