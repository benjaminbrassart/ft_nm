/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_qsort.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 19:28:20 by bbrassar          #+#    #+#             */
/*   Updated: 2024/07/23 05:06:04 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ext.h"

struct array {
	unsigned char *data;
	size_t memb_size;
};

// TODO optimize
static void _swap(struct array *arr, size_t a, size_t b) {
	unsigned char *p = arr->data + a * arr->memb_size;
	unsigned char *q = arr->data + b * arr->memb_size;
	unsigned char tmp;

	for (size_t i = 0; i < arr->memb_size; i++) {
		tmp = p[i];
		p[i] = q[i];
		q[i] = tmp;
	}
}

static size_t _partition(struct array *arr, size_t low, size_t high, cmp_func_t *cmp) {
	unsigned char const *pivot = arr->data + high * arr->memb_size;
	size_t i = low;

	for (size_t j = low; j < high; j++) {
		if (cmp(arr->data + j * arr->memb_size, pivot) < 0) {
			_swap(arr, i, j);
			i++;
		}
	}

	_swap(arr, i, high);
	return i;
}

static void _quicksort(struct array *arr, size_t low, size_t high, cmp_func_t *cmp) {
	size_t pivot;

	if (low < high) {
		pivot = _partition(arr, low, high, cmp);

		if (pivot > 0) {
			_quicksort(arr, low, pivot - 1, cmp);
		}

		_quicksort(arr, pivot + 1, high, cmp);
	}
}

void ft_qsort(void *base, size_t nmemb, size_t size, cmp_func_t *cmp)
{
	if (nmemb <= 1) {
		return;
	}

	struct array array;

	array.data = base;
	array.memb_size = size;

	_quicksort(&array, 0, nmemb - 1, cmp);
}
