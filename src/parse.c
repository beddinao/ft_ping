#include <ft_ping.h>

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
	long	count_nm = strtol(v[*current_arg], &endptr, 0xa);
	if (errno == ERANGE
		|| endptr == v[*current_arg] || *endptr != '\0'
		|| count_nm < 0 || count_nm >= U32_MAX) {
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
	long	interval_nm = strtol(v[*current_arg], &endptr, 0xa);
	if (errno == ERANGE
		|| endptr == v[*current_arg] || *endptr != '\0'
		|| interval_nm < 0 || interval_nm >= U32_MAX) {
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
	long	timeout_nm = strtol(v[*current_arg], &endptr, 0xa);
	if (errno == ERANGE
		|| endptr == v[*current_arg] || *endptr != '\0'
		|| timeout_nm < 0 || timeout_nm >= U32_MAX) {
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
	long	tos_nm = strtol(v[*current_arg], &endptr, 0xa);
	if (errno == ERANGE
		|| endptr == v[*current_arg] || *endptr != '\0'
		|| tos_nm < 0 || tos_nm > U08_MAX) {
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
	long	ttl_nm = strtol(v[*current_arg], &endptr, 0xa);
	if (errno == ERANGE
		|| endptr == v[*current_arg] || *endptr != '\0'
		|| ttl_nm < 0 || ttl_nm > U08_MAX) {
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
