/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtoi.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/05 22:56:13 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"

int	ft_strtoi(char const *s, char **endptr)
{
	int	result;
	int	sign;

	result = 0;
	sign = 1;
	while (ft_isspace(*s))
		++s;
	if (*s == '+' || *s == '-')
		if (*s++ == '-')
			sign = -1;
	while (ft_isdigit(*s))
		result = result * 10 + *s++ - '0';
	if (endptr != NULL)
		*endptr = (char *)s;
	return (result * sign);
}
