/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 12:18:51 by bbrassar          #+#    #+#             */
/*   Updated: 2024/08/29 18:30:58 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

struct config {
	/** -a, --debug-syms */
	unsigned debug_symbols : 1;
	/** -g, --extern-only */
	unsigned extern_only : 1;
	/** -p, --no-sort */
	unsigned no_sort : 1;
	/** -r, --reverse-sort */
	unsigned reverse_sort : 1;
	/** -u, --undefined-only */
	unsigned undefined_only : 1;
	/** -V, --version */
	unsigned display_version : 1;
	/** */
	unsigned print_file_name : 1;
};

extern struct config const DEFAULT_CONFIG;
