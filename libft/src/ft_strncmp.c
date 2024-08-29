/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/22 13:41:22 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"

int	ft_strncmp(char const *s1, char const *s2, size_t n)
{
	unsigned char const	*b1;
	unsigned char const	*b2;

	if (n == 0)
		return (0);
	b1 = (unsigned char const *)s1;
	b2 = (unsigned char const *)s2;
	while (--n > 0 && *b1 != '\0' && *b2 != '\0' && *b1 == *b2)
	{
		b1++;
		b2++;
	}
	return (*b1 - *b2);
}
