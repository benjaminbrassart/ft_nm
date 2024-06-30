/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benjamin <benjamin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 14:08:35 by benjamin          #+#    #+#             */
/*   Updated: 2024/06/30 14:17:46 by benjamin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.h"

struct config const DEFAULT_CONFIG = {
	.debug_symbols = 0,
	.extern_only = 0,
	.no_sort = 0,
	.reverse_sort = 0,
	.undefined_only = 0,
	.display_version = 0,
	.unicode_display = UnicodeDisplayDefault,
};
