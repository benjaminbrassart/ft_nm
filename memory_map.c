/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_map.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 16:11:39 by bbrassar          #+#    #+#             */
/*   Updated: 2024/07/23 01:48:12 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "memory_map.h"

#include <stddef.h>
#include <stdint.h>

void const *mm_read(struct memory_map const *mm, size_t offset, size_t n)
{
	if (offset + n > mm->map_size) {
		return NULL;
	}

	return (void const *)((uintptr_t)mm->map + offset);
}

void const *mm_sub(struct memory_map const *mm, struct memory_map *new_mm, size_t offset, size_t n)
{
	if (new_mm == NULL) {
		return NULL;
	}

	if (offset + n >= mm->map_size) {
		return NULL;
	}

	new_mm->map = (void const *)((uintptr_t)mm->map + offset);
	new_mm->map_size = n;

	return new_mm->map;
}

#ifdef UNIT_TEST

#include <assert.h>
#include <stdio.h>

static void test_mm_read(void)
{
	char buffer[16] = "hello!";
	struct memory_map mm;

	mm.map = buffer;
	mm.map_size = sizeof(buffer);

	assert(mm_read(&mm, 0, 0) != NULL);
	assert(mm_read(&mm, 0, 16) != NULL);
	assert(mm_read(&mm, 16, 0) != NULL);
	{
		void const *r = mm_read(&mm, 4, 3);

		assert(r != NULL);
		assert(r == (void const *)((uintptr_t)mm.map + 4));
	}

	assert(mm_read(&mm, 16 + 1, 0) == NULL);
	assert(mm_read(&mm, 0, 16 + 1) == NULL);
	assert(mm_read(&mm, 0, 16 + 1) == NULL);
}

static void test_mm_sub(void)
{

}

int main(void)
{
	test_mm_read();
	test_mm_sub();
}

#endif
