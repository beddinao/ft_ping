#include <ft_ping.h>

_data	g_vars;

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

	g_vars.dest_ip = inet_ntoa(((struct sockaddr_in*)g_vars.dest->ai_addr)->sin_addr);
	printf("FT_PING %s (%s)\n", host, g_vars.dest_ip ? g_vars.dest_ip : "??");
	return True;
}

bool	set_requested_ip_options() {
	int	_i = 0;
	int	option;

	option = g_vars.input.is_set_ttl ? g_vars.input.ttl : def_ttl;
	_i = setsockopt(g_vars.sock, IPPROTO_IP, IP_TTL, &option, sizeof(option));

	if (g_vars.input.is_set_tos) {
		option = g_vars.input.tos;
		_i = setsockopt(g_vars.sock, IPPROTO_IP, IP_TOS, &option, sizeof(option));
	}

	option = 0;
	_i = setsockopt(g_vars.sock, IPPROTO_IP, IP_HDRINCL, &option, sizeof(option));

	return _i >= 0;
}

bool	check_root_privileges(uid_t r, uid_t e, uid_t s) {
	return getresuid(&r, &e, &s) < 0 || e;
}

void	signal_handler(int sig_num) {
	if (g_vars.sock > 0) close(g_vars.sock);
	if (g_vars.dest) freeaddrinfo(g_vars.dest);
	struct	timeval	en = {0};
	printf("\nquiting...\n");
	printf("=== %s stats ===\n", g_vars.dest_ip ? g_vars.dest_ip : "");
	printf("%i %spackets transmitted, %s%i %sreceived, %s%i %slost, %s%0.2f%% %spacket loss, %s",
		g_vars.sent_packets, WHT, NRM, g_vars.recv_packets, WHT, NRM,
		g_vars.sent_packets - g_vars.recv_packets, WHT, NRM,
		100.0 - (((double)g_vars.recv_packets/(double)g_vars.sent_packets) * 100.0),
		WHT, NRM);
	gettimeofday(&en, NULL);
	printf("%stime: %s%s%0.3f ms%s\n", WHT, NRM, UND, ((double)(en.tv_sec - g_vars.st.tv_sec) * 1000)
		+ ((double)(en.tv_usec - g_vars.st.tv_usec) / 1000), NRM);
	exit(sig_num);
}

int main(int c, char **v) {
	if (check_root_privileges(0, 0, 0)) {
		printf("ft_ping: error: need root privileges\n");
		return 1;
	}
	if (c < 2 || c > 0xff || (c >= 2 && !parse_params(c, v))) {
		display_help();
		return 1;
	}
	if (!resolve_addr(v[c-1]))
		return 1;

	signal(SIGINT, signal_handler);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	g_vars.sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
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
	
	ft_ping(&timeout, &interval);
	signal_handler(0);
}
