/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benjamin <benjamin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 10:39:06 by benjamin          #+#    #+#             */
/*   Updated: 2024/06/30 14:38:44 by benjamin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.h"
#include "options.h"
#include "version.h"

#include <stdlib.h>
#include <string.h>

#include <unistd.h>

int main(int argc, char const *argv[])
{
	struct config config = DEFAULT_CONFIG;

	switch (parse_options(&argc, argv, &config)) {
	case ParseConfigEarlyExit:
		return EXIT_SUCCESS;
	case ParseConfigFailure:
		return EXIT_FAILURE;
	case ParseConfigSuccess:
		break;
	}

	if (config.display_version) {
		write(STDOUT_FILENO, "ft_nm version ", 14);
		write(STDOUT_FILENO, VERSION_STRING, strlen(VERSION_STRING));
		write(STDOUT_FILENO,
			"\n"
			"\n"
			"Copyright (C) 2024 Benjamin Brassart\n"
			"\n"
			"This program is free and open source software.\n"
			"You may redistribute it under the terms of the MIT license.\n"
			"\n"
			"Report bugs and issues at https://github.com/bemjaminbrassart/ft_nm\n", 216);
		return EXIT_SUCCESS;
	}

	if (argc == 1) {
		argv[1] = "a.out";
		argc += 1;
	}
}
