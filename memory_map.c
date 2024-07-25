/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_map.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 16:11:39 by bbrassar          #+#    #+#             */
/*   Updated: 2024/07/25 10:04:04 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "memory_map.h"

#include <stddef.h>
#include <stdint.h>

bool mm_check(struct memory_map const *mm, void const *ptr, size_t length)
{
	uintptr_t const start = (uintptr_t)mm->start;
	uintptr_t const end = (uintptr_t)mm->end;
	uintptr_t const p_start = (uintptr_t)ptr;

#if SIZE_MAX > UINTPTR_MAX
	if (length > UINTPTR_MAX || length > (size_t)(UINTPTR_MAX - p_start)) {
		return false;
	}

	uintptr_t const p_end = p_start + (uintptr_t)length;
#else
	uintptr_t const p_end = p_start + length;
#endif

	return p_start >= start && p_end <= end;
}
