/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_map.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 16:10:26 by bbrassar          #+#    #+#             */
/*   Updated: 2024/07/21 21:09:18 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stddef.h>

struct memory_map {
	void const *map;
	size_t map_size;
};

void const *mm_read(struct memory_map const *mm, void const *base, size_t n);
