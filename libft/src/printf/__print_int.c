/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   __print_int.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/09 10:08:58 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"
#include "libft/private/ft_printf.h"
#include <unistd.h>

int	__print_int(t_printerface *pi, va_list ap)
{
	char	buffer[11];
	size_t	len;

	len = ft_itoa_s(va_arg(ap, int), buffer);
	return (__printerface_write(pi, buffer, len));
}
