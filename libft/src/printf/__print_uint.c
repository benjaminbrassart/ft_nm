/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   __print_uint.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/17 05:52:24 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"
#include "libft/private/ft_printf.h"
#include <unistd.h>

static char const	g_base[] = "0123456789";

int	__print_uint(t_printerface *pi, va_list ap)
{
	char			buffer[10];
	size_t			len;
	unsigned int	n;

	n = va_arg(ap, unsigned int);
	len = __uitoa_base_s(n, buffer, sizeof (buffer), g_base);
	return (__printerface_write(pi, buffer, len));
}
