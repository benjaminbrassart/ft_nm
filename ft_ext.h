/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ext.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 19:26:03 by bbrassar          #+#    #+#             */
/*   Updated: 2024/08/29 15:43:56 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/** libft extensions */

#pragma once

#include <stddef.h>

typedef int cmp_func_t(void const *, void const *);

void ft_qsort(void *base, size_t nmemb, size_t size, cmp_func_t *cmp);

int ft_isupper(int c);
