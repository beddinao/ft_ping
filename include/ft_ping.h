#ifndef FT_PING
# define FT_PING

# define	_GNU_SOURCE

# include <stdio.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <netdb.h>
# include <string.h>
# include <stdlib.h>
# include <netinet/ip_icmp.h>
# include <unistd.h>
# include <sys/select.h>
# include <errno.h>
# include <signal.h>
# include <sys/time.h>

// GENERAL PURPOSE MACROS
# define ft_ping_version	"0x1 <foundations.>"
# define def_packet_size	4000
# define def_ttl		116	
# define icmp_types		((const char*[]){ "echoreply", "", "", "destination_unreachable", "source_quench", "redirect_msg", "", "", "echo_request", "router_ad", "router_solic", "iphdr_time_to_live_exceeded", "bad_ip_header", "timestamp", "timestamp_reply", "info_request", "info_reply", "addr_mask_request", "addr_mask_reply"})

// ANSI COLORS
# define UND		"\033[4m"
# define CYN		"\x1B[36m"
# define NRM		"\x1B[0m"
# define WHT		"\x1B[37m"

// KBYTES UNITS IN BYTES
# define _64KB		0x10000
# define _32KB		0x8000
# define _16KB		0x4000
# define _08KB		0x2000
# define _04KB		0x1000

// UNSINGED INTEGER TYPES MAX VALUES
# define U64_MAX		0xffffffffffffffff
# define U32_MAX		0xffffffff
# define U16_MAX		0xffff
# define U08_MAX		0xff

# ifndef True 
# define True 		1
# endif

# ifndef False
# define False		0
# endif

typedef	int	bool;

typedef	struct {
	bool		version;		// -V / --version
	bool		help;		// -? / --help
	bool		verbose;		// -v / --verbose
	bool		flood;		// -f / --flood
	bool		numeric_only;	// -n / --numeric
	//
	bool		is_set_count;
	bool		is_set_interval;
	bool		is_set_timeout;
	bool		is_set_ttl;
	bool		is_set_tos;
	//
	uint32_t		count;		// -c / --count
	uint32_t		interval;		// -i / --interval
	uint32_t		timeout;		// -W / --timeout
	uint8_t		tos;		// -Q / --tos
	uint8_t		ttl;		// -t / --ttl 
}	_options;

typedef	struct {
	int		sock;
	struct	addrinfo	*dest;
	char		*dest_ip;
	char		hostname[def_packet_size];
	bool		resolved_hostname;
	uint64_t		sent_packets;
	uint64_t		recv_packets;
	struct	timeval	timeout;
	struct	timeval	st;
	struct	timeval	end;
	_options		input;
}	_data;

extern	_data	g_vars;

// print.c
void	display_help();
void	print_outgoing_packet();
void	print_incoming_packet(struct sockaddr_in*, struct icmphdr*, struct icmphdr*, uint16_t, struct timeval*, struct timeval*, bool);

// parse.c
bool	parse_params(int, char**);

// ft_ping.c
void	ft_ping(struct timeval*, struct timeval *);

#endif
