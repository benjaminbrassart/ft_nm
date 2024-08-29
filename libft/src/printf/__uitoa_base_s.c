/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   __uitoa_base_s.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/17 06:01:17 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"
#include "libft/private/ft_printf.h"

size_t	__uitoa_base_s(
	unsigned long long n,
	char *buffer,
	size_t buffer_size,
	char const *base
)
{
	size_t const	base_len = ft_strlen(base);
	size_t			len;

	len = 0;
	while (buffer_size >= len && (len == 0 || n > 0))
	{
		buffer[buffer_size - (++len)] = base[n % base_len];
		n /= base_len;
	}
	if (n > 0)
		len = 0;
	else
		ft_memmove(buffer, buffer + (buffer_size - len), len);
	return (len);
}
