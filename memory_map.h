/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_map.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 16:10:26 by bbrassar          #+#    #+#             */
/*   Updated: 2024/07/23 01:50:15 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stddef.h>

struct memory_map {
	void const *map;
	size_t map_size;
};

/**
 * Check bounds for a memory map object, and return an offset pointer
 */
void const *mm_read(struct memory_map const *mm, size_t offset, size_t n);

/**
 * Create a new bounds-checked memory map object.
 */
void const *mm_sub(struct memory_map const *mm, struct memory_map *new_mm, size_t offset, size_t n);
