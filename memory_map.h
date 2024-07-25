/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_map.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 16:10:26 by bbrassar          #+#    #+#             */
/*   Updated: 2024/07/25 12:12:08 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdbool.h>
#include <stddef.h>

struct memory_map {
	void const *start;
	void const *end;
};

#define mm_new(Ptr, Size) { \
	.start = Ptr, \
	.end = (void const *)((uint8_t const *)Ptr + Size), \
}

/**
 * Check whether a pointer is within bounds of a memory map
 */
bool mm_check(struct memory_map const *mm, void const *ptr, size_t length);
