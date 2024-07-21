/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_qsort.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 19:28:20 by bbrassar          #+#    #+#             */
/*   Updated: 2024/07/21 22:28:29 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ext.h"

#include <stdlib.h>

void ft_qsort(void *base, size_t nmemb, size_t size, cmp_func_t *cmp)
{
	// TODO implement ft_qsort
	qsort(base, nmemb, size, cmp);
}
