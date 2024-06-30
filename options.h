/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benjamin <benjamin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 12:30:25 by benjamin          #+#    #+#             */
/*   Updated: 2024/06/30 14:07:43 by benjamin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "config.h"

#include <stdbool.h>

enum parse_config_result {
	ParseConfigSuccess,
	ParseConfigFailure,
	ParseConfigEarlyExit,
};

enum option_value_policy {
	OptionValueRequired,
	OptionValueOptional,
	OptionValueDenied,
};

enum option_name {
	OptionDebugSymbols,
	OptionExternOnly,
	OptionNoSort,
	OptionReverseSort,
	OptionUndefinedOnly,
	OptionUnicode,
	OptionHelp,
	OptionVersion,
};

struct option {
	enum option_name name;
	int opt_short;
	char const *opt_long;
	enum option_value_policy value_policy;
	char const *description;
	bool early_exit;
};

enum parse_config_result parse_options(int *argc, char const *argv[], struct config *config);
