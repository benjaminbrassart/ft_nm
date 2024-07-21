/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_map.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 16:11:39 by bbrassar          #+#    #+#             */
/*   Updated: 2024/07/21 21:09:18 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "memory_map.h"

#include <stddef.h>

void const *mm_read(struct memory_map const *mm, void const *base, size_t n)
{
	if (base == NULL) {
		base = mm->map;
	}

	if (base < mm->map || base + n > mm->map + mm->map_size) {
		return NULL;
	}

	return base;
}
