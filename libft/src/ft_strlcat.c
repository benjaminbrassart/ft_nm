/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 11:46:01 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"

size_t	ft_strlcat(char *dst, char const *src, size_t dstsize)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (dst[i] != '\0' && i <= dstsize)
		++i;
	if (i >= dstsize)
		return (ft_strlen(src) + dstsize);
	j = 0;
	while (src[j] != '\0' && i + j < dstsize - 1)
	{
		dst[i + j] = src[j];
		++j;
	}
	dst[i + j] = 0;
	return (ft_strlen(src) + i);
}
