#ifndef FT_PING
# define FT_PING

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

# define ft_ping_version	"0x1 <foundations.>"
# define PK_SIZE		4000
# define icmp_types		((const char*[]){ "echoreply", "", "", "destination_unreachable", "source_quench", "redirect_msg", "", "", "echo_request", "router_ad", "time_exceed", "bad_ip_header", "timestamp", "timestamp_reply", "info_request", "info_reply", "addr_mask_request", "addr_mask_reply"})
# define UND		"\033[4m"
# define CYN		"\x1B[36m"
# define NRM		"\x1B[0m"
# define WHT		"\x1B[37m"

#ifndef True 
# define True 1
#endif
#ifndef False
# define False 0
#endif

// ./ping --count 1 :: 3
//

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
	uint32_t		count;		// -c / --count < 0xffffffff
	uint8_t		interval;		// -i / --interval
	uint32_t		timeout;		// -W / --timeout
	uint8_t		tos;		// -Q / --tos
	uint8_t		ttl;		// -t / --ttl 
}	_options;

typedef	struct {
	int		sock;
	struct	addrinfo	*dest;
	char		*dest_ip;
	int		sent_packets;
	int		recv_packets;
	struct	timeval	timeout;
	struct	timeval	st;
	struct	timeval	end;
	_options		input;
}	_data;

extern	_data	g_vars;

#endif
