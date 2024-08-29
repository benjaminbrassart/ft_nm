/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_dprintf.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/09 09:53:29 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"
#include "libft/private/ft_printf.h"
#include "libft/private/conversions.h"
#include <unistd.h>

int	ft_dprintf(int fd, char const *fmt, ...)
{
	int		res;
	va_list	ap;

	va_start(ap, fmt);
	res = ft_vdprintf(fd, fmt, ap);
	va_end(ap);
	return (res);
}

int	ft_vdprintf(int fd, char const *fmt, va_list ap)
{
	struct s_printerface	pi;

	pi.iface.fd = fd;
	pi.type = PF_FILE_DESCRIPTOR;
	return (ft_printf_core(&pi, fmt, ap));
}
