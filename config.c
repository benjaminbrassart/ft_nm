/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 14:08:35 by bbrassar          #+#    #+#             */
/*   Updated: 2024/08/29 18:31:03 by bbrassar         ###   ########.fr       */
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
};
