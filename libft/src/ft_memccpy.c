/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memccpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/22 13:45:49 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"

void	*ft_memccpy(void *dst, void const *src, int c, size_t n)
{
	unsigned char		*d;
	unsigned char const	*s;
	unsigned char const	ch = c;

	d = dst;
	s = src;
	while (n-- > 0)
	{
		*d++ = *s;
		if (*s++ == ch)
			return (d);
	}
	return (NULL);
}
