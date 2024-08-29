/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/03 02:39:29 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"

char	*ft_strrstr(char const *haystack, char const *needle)
{
	size_t const	needle_len = ft_strlen(needle);
	size_t			i;
	size_t			j;

	i = ft_strlen(haystack);
	while (i >= needle_len)
	{
		j = needle_len;
		while (j && haystack[i - j] == needle[needle_len - j])
			--j;
		if (j == 0)
			return ((char *)haystack + i - needle_len);
		--i;
	}
	return (NULL);
}
