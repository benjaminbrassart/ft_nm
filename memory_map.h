/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_map.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benjamin <benjamin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 16:10:26 by benjamin          #+#    #+#             */
/*   Updated: 2024/06/30 16:11:29 by benjamin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stddef.h>

struct memory_map {
	void const *map;
	size_t map_size;
};

void const *mm_read(struct memory_map const *mm, void const *base, size_t n);
