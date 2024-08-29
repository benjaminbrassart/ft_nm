/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   __print_string.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/09 10:06:38 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"
#include "libft/private/ft_printf.h"
#include <unistd.h>

int	__print_string(t_printerface *pi, va_list ap)
{
	char const	*s = va_arg(ap, char const *);

	if (s == NULL)
		s = "(null)";
	return (__printerface_write(pi, s, ft_strlen(s)));
}
