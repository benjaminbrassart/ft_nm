/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 11:00:24 by bbrassar          #+#    #+#             */
/*   Updated: 2024/07/21 21:09:18 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.h"
#include "options.h"

#include "libft/ft.h"

#include <stdbool.h>
#include <stdlib.h>

#include <unistd.h>

#define sizeof_array(Array) (sizeof(Array)/sizeof(Array[0]))

static struct option const OPTIONS[] = {
	{
		.name = OptionDebugSymbols,
		.opt_short = 'a',
		.opt_long = "debug-syms",
		.value_policy = OptionValueDenied,
		.description = "Display debugger-only symbols",
		.early_exit = false,
	},
	{
		.name = OptionExternOnly,
		.opt_short = 'g',
		.opt_long = "extern-only",
		.value_policy = OptionValueDenied,
		.description = "Display only external symbols",
		.early_exit = false,
	},
	{
		.name = OptionNoSort,
		.opt_short = 'p',
		.opt_long = "no-sort",
		.value_policy = OptionValueDenied,
		.description = "Do not sort the symbols",
		.early_exit = false,
	},
	{
		.name = OptionReverseSort,
		.opt_short = 'r',
		.opt_long = "reverse-sort",
		.value_policy = OptionValueDenied,
		// weird sentence from gnu nm
		.description = "Reverse the sense of the sort",
		.early_exit = false,
	},
	{
		.name = OptionUndefinedOnly,
		.opt_short = 'u',
		.opt_long = "undefined-only",
		.value_policy = OptionValueDenied,
		.description = "Display only undefined symbols",
		.early_exit = false,
	},
	{
		.name = OptionUnicode,
		.opt_short = 0,
		.opt_long = "unicode",
		.value_policy = OptionValueRequired,
		.description = "Specify how to treat UTF-8 encoded unicode characters",
		.early_exit = false,
	},
	{
		.name = OptionHelp,
		.opt_short = 'h',
		.opt_long = "help",
		.value_policy = OptionValueDenied,
		.description = "Display help information and exit",
		.early_exit = true,
	},
	{
		.name = OptionVersion,
		.opt_short = 'V',
		.opt_long = "version",
		.value_policy = OptionValueDenied,
		.description = "Display version and exit",
		.early_exit = false,
	},
};

static void _display_help(int fd)
{
	static char padding_buffer[80];

	ft_memset(padding_buffer, ' ', sizeof(padding_buffer));

	char line_buffer[81];
	static char const usage[] = \
		"Usage: nm [option(s)] [file(s)]\n" \
		" List symbols in [file(s)] (a.out by default).\n" \
		" The options are:\n"
	;

	write(STDERR_FILENO, usage, sizeof(usage) - 1);

	size_t longest_option_name = 0;
	size_t option_name_len;

	struct option const *opt;

	for (size_t i = 0; i < sizeof_array(OPTIONS); i += 1) {
		opt = &OPTIONS[i];

		if (opt->opt_long != NULL) {
			option_name_len = ft_strlen(opt->opt_long);
			if (option_name_len > longest_option_name) {
				longest_option_name = option_name_len;
			}
		}
	}

	size_t long_padding;
	size_t buffer_len;

	for (size_t i = 0; i < sizeof_array(OPTIONS); i += 1) {
		opt = &OPTIONS[i];
		ft_memset(line_buffer, '\0', sizeof(line_buffer));

		if (opt->opt_short != 0) {
			ft_strlcat(line_buffer, "  -?, ", sizeof(line_buffer));
			line_buffer[3] = (char)opt->opt_short;
		} else {
			ft_strlcat(line_buffer, "      ", sizeof(line_buffer));
		}

		if (opt->opt_long != NULL) {
			option_name_len = ft_strlen(opt->opt_long);
			ft_strlcat(line_buffer, "--", sizeof(line_buffer));
			ft_strlcat(line_buffer, opt->opt_long, sizeof(line_buffer));
			long_padding = longest_option_name - option_name_len;
		} else {
			long_padding = longest_option_name;
		}

		ft_strlcat(line_buffer, padding_buffer + (sizeof(padding_buffer) - (long_padding + 1)), sizeof(line_buffer));

		char const *description = opt->description;
		size_t desc_offset = longest_option_name + 9;
		size_t desc_frag_len;

		while (*description != '\0') {
			if (description != opt->description) {
				ft_strlcpy(line_buffer, padding_buffer, desc_offset);
				line_buffer[desc_offset] = '\0';
			}
			desc_frag_len = ft_strnlen(description, 80 - desc_offset);

			ft_strlcat(line_buffer, description, sizeof(line_buffer));
			description += desc_frag_len;
			buffer_len = ft_strlen(line_buffer);
			line_buffer[buffer_len] = '\n';
			write(fd, line_buffer, buffer_len + 1);
		}
	}
}

static int _handle_option(enum option_name name, char const *value, struct config *conf)
{
	switch (name) {
	case OptionDebugSymbols:
		conf->debug_symbols = 1;
		break;
	case OptionExternOnly:
		conf->extern_only = 1;
		break;
	case OptionNoSort:
		conf->no_sort = 1;
		break;
	case OptionReverseSort:
		conf->reverse_sort = 1;
		break;
	case OptionUndefinedOnly:
		conf->undefined_only = 1;
		break;
	case OptionUnicode:
		if (ft_strcmp(value, "default") == 0) {
			conf->unicode_display = UnicodeDisplayDefault;
		} else if (ft_strcmp(value, "show") == 0) {
			conf->unicode_display = UnicodeDisplayShow;
		} else if (ft_strcmp(value, "invalid") == 0) {
			conf->unicode_display = UnicodeDisplayInvalid;
		} else if (ft_strcmp(value, "hex") == 0) {
			conf->unicode_display = UnicodeDisplayHex;
		} else if (ft_strcmp(value, "escape") == 0) {
			conf->unicode_display = UnicodeDisplayEscape;
		} else if (ft_strcmp(value, "highlight") == 0) {
			conf->unicode_display = UnicodeDisplayHighlight;
		} else {
			write(STDERR_FILENO, "ft_nm: invalid argument to -U/--unicode: ", 41);
			write(STDERR_FILENO, value, ft_strlen(value));
			write(STDERR_FILENO, "\n", 1);
			return EXIT_FAILURE;
		}
		break;
	case OptionVersion:
		conf->display_version = 1;
		break;
	case OptionHelp:
		_display_help(STDOUT_FILENO);
		break;
	default:
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

static void _shift(int index, int *argc, char const *argv[])
{
	for (int i = index; i < *argc; i += 1) {
		argv[i] = argv[i + 1];
	}
	*argc -= 1;
}

// static struct option const *_get_option_short();
static struct option const *_get_option_long(char const *name, char const **opt_val)
{
	char const *eq = ft_strchr(name, '=');
	size_t cmp_len;

	if (eq == NULL) {
		cmp_len = ft_strlen(name);
		*opt_val = NULL;
	} else {
		cmp_len = (size_t)(eq - name);
		*opt_val = eq + 1;
	}

	for (size_t i = 0; i < sizeof_array(OPTIONS); i += 1) {
		if (ft_strncmp(OPTIONS[i].opt_long, name, cmp_len) == 0 && OPTIONS[i].opt_long[cmp_len] == '\0') {
			return &OPTIONS[i];
		}
	}

	return NULL;
}

static struct option const *_get_option_short(int name)
{
	for (size_t i = 0; i < sizeof_array(OPTIONS); i += 1) {
		if (OPTIONS[i].opt_short == (char)name) {
			return &OPTIONS[i];
		}
	}

	return NULL;
}

static bool _is_option(bool ddash, char const *s)
{
	return !ddash && s[0] == '-' && s[1] != '\0';
}

static void _fetch_option(int index, int *argc, char const *argv[], char const **opt_val, bool soft)
{
	if ((index + 1) >= *argc) {
		*opt_val = NULL;
		return;
	}

	char const *next_arg = argv[index + 1];

	if (_is_option(false, next_arg) && soft) {
		*opt_val = NULL;
	} else {
		*opt_val = next_arg;
		_shift(index + 1, argc, argv);
	}
}

enum parse_config_result parse_options(int *argc, char const *argv[], struct config *config)
{
	struct option const *opt;
	char const *opt_val;
	bool ddash;
	int i;

	ddash = false;
	i = 1;
	while (i < *argc) {
		if (!_is_option(ddash, argv[i])) {
			i += 1;
			continue;
		}

		opt_val = NULL;
		opt = NULL;
		if (argv[i][1] == '-') {
			if (argv[i][2] == '\0') {
				ddash = true;
				_shift(i, argc, argv);
				continue;
			}

			opt = _get_option_long(&argv[i][2], &opt_val);
			if (opt == NULL) {
				write(STDERR_FILENO, "ft_nm: unrecognized option '", 28);
				write(STDERR_FILENO, argv[i], ft_strlen(argv[i]));
				write(STDERR_FILENO, "'\n", 2);
				_display_help(STDERR_FILENO);
				return ParseConfigFailure;
			}

			switch (opt->value_policy) {
			case OptionValueRequired:
				if (opt_val == NULL) {
					_fetch_option(i, argc, argv, &opt_val, false);
				}

				if (opt_val == NULL) {
					write(STDERR_FILENO, "ft_nm: option '", 15);
					write(STDERR_FILENO, argv[i], ft_strlen(opt->opt_long) + 2);
					write(STDERR_FILENO, "' requires an argument\n", 23);
					_display_help(STDERR_FILENO);
					return ParseConfigFailure;
				}

				break;

			case OptionValueOptional:
				if (opt_val == NULL) {
					_fetch_option(i, argc, argv, &opt_val, true);
				}

				break;

			case OptionValueDenied:
				if (opt_val != NULL) {
					write(STDERR_FILENO, "ft_nm: option '", 15);
					write(STDERR_FILENO, argv[i], ft_strlen(opt->opt_long) + 2);
					write(STDERR_FILENO, "' doesn't allow an argument\n", 28);
					_display_help(STDERR_FILENO);
					return ParseConfigFailure;
				}
				break;
			}

			if (_handle_option(opt->name, opt_val, config) != EXIT_SUCCESS) {
				return ParseConfigFailure;
			}

			if (opt->early_exit) {
				return ParseConfigEarlyExit;
			}
		} else {
			for (int j = 1; argv[i][j] != '\0'; j += 1) {
				opt = _get_option_short(argv[i][j]);
				if (opt == NULL) {
					write(STDERR_FILENO, "ft_nm: invalid option -- '", 26);
					write(STDERR_FILENO, &argv[i][j], 1);
					write(STDERR_FILENO, "'\n", 2);
					_display_help(STDERR_FILENO);
					return ParseConfigFailure;
				}

				if (_handle_option(opt->name, NULL, config) != EXIT_SUCCESS) {
					return ParseConfigFailure;
				}

				if (opt->early_exit) {
					return ParseConfigEarlyExit;
				}
			}
		}
		_shift(i, argc, argv);
	}

	return ParseConfigSuccess;
}
