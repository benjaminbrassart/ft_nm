/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_snprintf.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/17 07:13:09 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"
#include "libft/private/ft_printf.h"

int	ft_snprintf(char *buffer, size_t size, char const *fmt, ...)
{
	va_list	ap;
	int		res;

	va_start(ap, fmt);
	res = ft_vsnprintf(buffer, size, fmt, ap);
	va_end(ap);
	return (res);
}

int	ft_vsnprintf(char *buffer, size_t size, char const *fmt, va_list ap)
{
	struct s_printerface	pi;

	pi.iface.buffer.buffer = buffer;
	pi.iface.buffer.current_size = 0;
	pi.iface.buffer.max_size = size;
	pi.type = PF_STACK_BUFFER;
	return (ft_printf_core(&pi, fmt, ap));
}
