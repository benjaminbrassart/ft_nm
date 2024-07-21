/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_qsort.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 19:28:20 by bbrassar          #+#    #+#             */
/*   Updated: 2024/07/22 01:20:48 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ext.h"

struct array {
	unsigned char *data;
	size_t memb_count;
	size_t memb_size;
};

// TODO optimize
static void _swap(void *b1, void *b2, size_t n)
{
	unsigned char temp;
	unsigned char *s1 = b1;
	unsigned char *s2 = b2;

	for (size_t i = 0; i < n; i += 1) {
		temp = s1[i];
		s1[i] = s2[i];
		s2[i] = temp;
	}
}

static size_t _part(struct array *array, size_t start, size_t end, cmp_func_t *cmp)
{
	size_t memb_size = array->memb_size;
	void *pivot = &array->data[memb_size * end];
	size_t pivot_index = start;

	for(size_t i = start; i < end; i++) {
		if(cmp(&array->data[memb_size * i], pivot) <= 0) {
			_swap(
				&array->data[memb_size * i],
				&array->data[memb_size * pivot_index],
				memb_size
			);
			pivot_index++;
		}
	}

	_swap(
		&array->data[memb_size * end],
		&array->data[memb_size * pivot_index],
		memb_size
	);

	//at last returning the pivot value index
	return pivot_index;
}

static void _sort(struct array *array, size_t start, size_t end, cmp_func_t *cmp)
{
	size_t pivot_index;

	if (start >= end) {
		return;
	}

	pivot_index = _part(array, start, end, cmp);
	_sort(array, start, pivot_index - 1, cmp);
	_sort(array, pivot_index + 1, end, cmp);
}

void ft_qsort(void *base, size_t nmemb, size_t size, cmp_func_t *cmp)
{
	struct array array;

	array.data = base;
	array.memb_count = nmemb;
	array.memb_size = size;

	_sort(&array, 0, nmemb, cmp);
}
