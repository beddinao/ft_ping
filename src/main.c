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
	printf("                              default: 10000 -> 10s\n");
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
	if (g_vars.input.verbose) { 
		printf("WARN: --verbose will be ignored due to the flood option\n");
		g_vars.input.verbose = False;
	}
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
		for (uint8_t current_possible_param = 0; current_possible_param < 0x8; current_possible_param += 1) {
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

bool	resolve_addr(char *host) {
	struct	addrinfo	hints;
	int		i;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	i = getaddrinfo(host, NULL, &hints, &g_vars.dest);
	if (i != 0 || !g_vars.dest) {
		printf("ft_ping: %s: Name or service not known\n", host);
		return False;
	}

	if (!g_vars.input.numeric_only) {
		g_vars.dest_ip = inet_ntoa(((struct sockaddr_in*)g_vars.dest->ai_addr)->sin_addr);
		if (g_vars.dest_ip)
			printf("%s (%s)\n", host, g_vars.dest_ip);
	}

	return True;
}

void	print_input_options() {
	printf("\nBOOLS:\n--version: %i\n--help: %i\n--verbose: %i\n--flood: %i\n--numeric_only: %i\n--is_set_count: %i\n--is_set_internval: %i\n--is_set_timeout: %i\n--is_set_ttl: %i\n--is_set_tos: %i\n\n",
			g_vars.input.version, g_vars.input.help, g_vars.input.verbose, g_vars.input.flood, g_vars.input.numeric_only, g_vars.input.is_set_count, g_vars.input.is_set_interval, g_vars.input.is_set_timeout, g_vars.input.is_set_ttl, g_vars.input.is_set_tos);

	printf("\nVALUES:\n--count: %i\n--interval: %i\n--timeout: %i\n--tos: %i\n--ttl: %i\n\n",
			g_vars.input.count, g_vars.input.interval, g_vars.input.timeout, g_vars.input.tos, g_vars.input.ttl);
}

bool	set_requested_ip_options() {
	int	_i = 0;
	int	option;

	if (g_vars.input.is_set_ttl) {
		option = g_vars.input.ttl;
		_i = setsockopt(g_vars.sock, IPPROTO_IP, IP_TTL, &option, sizeof(option));
	}

	if (g_vars.input.is_set_tos) {
		option = g_vars.input.tos;
		_i = setsockopt(g_vars.sock, IPPROTO_IP, IP_TOS, &option, sizeof(option));
	}

	return _i >= 0;
}

void	print_outgoing_packet() {
	if (g_vars.input.flood)
		write(1, ".", 1);
}

void	print_incoming_packet(struct icmphdr *icmphdr, struct timeval *start, struct timeval *end) {
	if (g_vars.input.flood) {
		write(1, "\b", 1);
		return;
	}
	if (icmphdr->type != ICMP_ECHOREPLY && !g_vars.input.verbose)
		return;
	printf("%s t=%0.3f ms, echo.id=%i, icmp_seq=%i\n",
		icmphdr->type == ICMP_ECHOREPLY ? "icmp_echoreply": "icmp_reply",
		((double)(end->tv_sec - start->tv_sec) * 1000) + ((double)(end->tv_usec - start->tv_usec) / 1000),
		icmphdr->un.echo.id, icmphdr->un.echo.sequence);
}

void	ft_ping(struct timeval *timeout, struct timeval *interval) {
	char		packet_out[PK_SIZE], packet_in[PK_SIZE];
	struct	icmphdr	*icmphdr_out;
	struct	icmphdr	icmphdr_in;
	struct	timeval	timeval_st;
	struct	timeval	timeval_end;
	socklen_t		addr_len = sizeof(struct sockaddr);
	uint16_t		icmphdr_len = sizeof(struct icmphdr);
	uint16_t		sequence = 0, _ops_res, _ops_inc;
	uint16_t		ops_id = htons(getpid());
	fd_set		r_set;

	g_vars.sent_packets = 0;
	for (;;) {
		/////// // // SENDING
		icmphdr_out = (struct icmphdr*)packet_out;
		memset(packet_out, 0, sizeof(packet_out));
		icmphdr_out->type = ICMP_ECHO;
		icmphdr_out->un.echo.id = ops_id;
		icmphdr_out->un.echo.sequence = sequence++;
		icmphdr_out->checksum = csum((unsigned short *)packet_out, icmphdr_len/2);

		gettimeofday(&timeval_st, NULL);
		_ops_res = sendto(g_vars.sock, packet_out, icmphdr_len, 0, g_vars.dest->ai_addr, addr_len);
		if (_ops_res < 0) {
			perror("ft_ping: sendto()");
			break;
		}
		print_outgoing_packet();
		g_vars.sent_packets += 1;
		if (g_vars.input.is_set_count && g_vars.sent_packets >= g_vars.input.count)
			break;

		//// // / /// WAITING FOR RESPONSE
		FD_ZERO(&r_set);
		FD_SET(g_vars.sock, &r_set);
		_ops_res = select(g_vars.sock + 1, &r_set, NULL, NULL, timeout);
		if (_ops_res < 0) {
			perror("ft_ping: select()");
			break;
		}
		else if (!_ops_res) {
			if (!g_vars.input.flood) {
				printf("## timeout [%lis] retrying in 1s..\n", timeout->tv_sec);
				sleep(1);
			}
			continue;
		}
		gettimeofday(&timeval_end, NULL);
		
		//// / // /// READING RESPONSE
		memset(packet_out, 0, sizeof(packet_out));
		_ops_inc = 0;
		for (_ops_inc = 0; _ops_inc < icmphdr_len; _ops_inc += _ops_res) {
			memset(packet_in, 0, sizeof(packet_in));
			_ops_res = recvfrom(g_vars.sock, packet_in, PK_SIZE, 0, g_vars.dest->ai_addr, &addr_len);
			if (_ops_res < 0) {
				perror("ft_ping: recvfrom()");
				break;
			}
			else if (!_ops_res) {
				if (_ops_inc < icmphdr_len) 
					continue;
				break;
			}
			memcpy(packet_out + _ops_inc, packet_in, _ops_res);
		}
		if (_ops_res < 0) break;

		memset(&icmphdr_in, 0, sizeof(icmphdr_in));
		memcpy(&icmphdr_in, packet_out, icmphdr_len);
		
		print_incoming_packet(&icmphdr_in, &timeval_st, &timeval_end);
		usleep((interval->tv_sec * 1000000) + interval->tv_usec);
	}
}

int main(int c, char **v) {
	if (c < 2 || c > 0xff || (c >= 2 && !parse_params(c, v))) {
		display_help();
		return 1;
	}
	if (!resolve_addr(v[c-1]))
		return 1;

	signal(SIGINT, exit);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	g_vars.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
	if (g_vars.sock < 0 || !set_requested_ip_options()) {
		perror("ft_ping");
		return 1;
	}

	struct	timeval	timeout = {0};
	struct	timeval	interval = {0};

	interval.tv_sec = g_vars.input.is_set_interval ? g_vars.input.interval : !g_vars.input.flood;
	if (g_vars.input.is_set_timeout) {
		timeout.tv_sec = g_vars.input.timeout / 1000;
		timeout.tv_usec = (g_vars.input.timeout % 1000) * 1000;
	}
	else	timeout.tv_sec = 10;
	gettimeofday(&g_vars.st, NULL);
	
	//print_input_options();

	ft_ping(&timeout, &interval);
}
