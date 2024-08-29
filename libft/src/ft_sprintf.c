/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_sprintf.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/17 07:26:23 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"
#include "libft/private/ft_printf.h"
#include <limits.h>

int	ft_sprintf(char *buffer, char const *fmt, ...)
{
	va_list	ap;
	int		res;

	va_start(ap, fmt);
	res = ft_vsprintf(buffer, fmt, ap);
	va_end(ap);
	return (res);
}

int	ft_vsprintf(char *buffer, char const *fmt, va_list ap)
{
	struct s_printerface	pi;

	pi.iface.buffer.buffer = buffer;
	pi.iface.buffer.current_size = 0;
	pi.iface.buffer.max_size = ULLONG_MAX;
	pi.type = PF_STACK_BUFFER;
	return (ft_printf_core(&pi, fmt, ap));
}
