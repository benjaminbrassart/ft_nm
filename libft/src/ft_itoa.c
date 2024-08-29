/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 14:00:56 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"

size_t	ft_itoa_s(int i, char *buffer)
{
	unsigned int	n;
	size_t			count;

	if (i < 0)
		n = -i;
	else
		n = i;
	count = 0;
	while (n > 0 || count == 0)
	{
		buffer[11 - ++count] = n % 10 + '0';
		n /= 10;
	}
	if (i < 0)
		buffer[11 - ++count] = '-';
	ft_memmove(buffer, buffer + (11 - count), count);
	return (count);
}

char	*ft_itoa(int i)
{
	char			buffer[11];
	size_t const	len = ft_itoa_s(i, buffer);

	return (ft_strndup(buffer, len));
}
