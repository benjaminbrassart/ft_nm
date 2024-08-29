/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 14:55:25 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

void	*ft_calloc(size_t count, size_t size)
{
	void	*mem;

	if (count > (ULONG_MAX / size))
	{
		errno = ERANGE;
		return (NULL);
	}
	mem = malloc(count * size);
	if (mem != NULL)
		ft_memset(mem, 0, count * size);
	return (mem);
}
