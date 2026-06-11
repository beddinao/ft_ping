#include <ft_ping.h>

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
	printf("                              0xff <= <tos> >= 0\n");
	printf("                              default: 0\n");
	printf("  -t --ttl <ttl>            set Time To Live <ttl> bits\n");
	printf("                              0xff <= <ttl> >= 0\n");
	printf("                              default: 64\n");
	printf("  -f --flood                flood ping\n");
	printf("  -n --numeric              no dns name resolution\n");
	printf("  -v --verbose              verbose output\n");
	printf("  -V --version              prints version and exits\n");
	printf("  -? --help                 shows this help and exits\n\n");
}

void	print_outgoing_packet() {
	if (g_vars.input.flood)
		write(1, ".", 1);
}

void	print_incoming_packet(struct icmphdr *icmphdr, uint16_t received, struct timeval *start, struct timeval *end, bool valid_csum) {
	if (g_vars.input.flood) {
		write(1, "\b", 1);
		return;
	}
	/*if (icmphdr->type != ICMP_ECHOREPLY && !g_vars.input.verbose)
		return;*/
	if (!valid_csum && !g_vars.input.verbose)
		return;

	bool	hostname_ready = 1;

	if (!g_vars.input.numeric_only) {
		if (!g_vars.resolved_hostname) {
			memset(g_vars.hostname, 0, sizeof(g_vars.hostname));
			hostname_ready = getnameinfo(g_vars.dest->ai_addr, sizeof(struct addrinfo), g_vars.hostname, sizeof(g_vars.hostname), NULL, 0, 0);
			g_vars.resolved_hostname = True;
		}
		else 	hostname_ready = 0;
	}

	printf("%u bytes from %s (%s) %s type=icmp_%s, echo.id=%i, icmp_seq=%i t=%0.3f ms\n",
		received,
		(g_vars.input.numeric_only || hostname_ready) ? "\b" : g_vars.hostname,
		g_vars.dest_ip,
		valid_csum ? "\b" : "(invalid_checksum)",
		icmphdr->type < 18 ? icmp_types[icmphdr->type] : "",
		icmphdr->un.echo.id, icmphdr->un.echo.sequence,
		((double)(end->tv_sec - start->tv_sec) * 1000) + ((double)(end->tv_usec - start->tv_usec) / 1000));
}

