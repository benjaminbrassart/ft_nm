/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 12:18:51 by benjamin          #+#    #+#             */
/*   Updated: 2024/07/21 19:46:48 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

enum unicode_display {
	UnicodeDisplayDefault,
	UnicodeDisplayShow,
	UnicodeDisplayInvalid,
	UnicodeDisplayHex,
	UnicodeDisplayEscape,
	UnicodeDisplayHighlight,
};

struct config {
	/** -a, --debug-syms */
	unsigned debug_symbols   : 1;
	/** -g, --extern-only */
	unsigned extern_only     : 1;
	/** -p, --no-sort */
	unsigned no_sort         : 1;
	/** -r, --reverse-sort */
	unsigned reverse_sort    : 1;
	/** -u, --undefined-only */
	unsigned undefined_only  : 1;
	/** -V, --version */
	unsigned display_version : 1;
	/** --unicode */
	enum unicode_display unicode_display;
};

extern struct config const DEFAULT_CONFIG;
