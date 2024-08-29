/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/09 09:39:30 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"
#include <unistd.h>

int	ft_printf(char const *fmt, ...)
{
	int		res;
	va_list	ap;

	va_start(ap, fmt);
	res = ft_vprintf(fmt, ap);
	va_end(ap);
	return (res);
}

int	ft_vprintf(char const *fmt, va_list ap)
{
	return (ft_vdprintf(STDOUT_FILENO, fmt, ap));
}
