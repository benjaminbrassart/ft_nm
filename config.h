/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benjamin <benjamin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 12:18:51 by benjamin          #+#    #+#             */
/*   Updated: 2024/06/30 14:17:53 by benjamin         ###   ########.fr       */
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
	unsigned debug_symbols   : 1;
	unsigned extern_only     : 1;
	unsigned no_sort         : 1;
	unsigned reverse_sort    : 1;
	unsigned undefined_only  : 1;
	unsigned display_version : 1;
	enum unicode_display unicode_display;
};

extern struct config const DEFAULT_CONFIG;
