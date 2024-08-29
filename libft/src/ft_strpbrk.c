/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strpbrk.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/05 22:46:16 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"

char	*ft_strpbrk(char const *s, char const *accept)
{
	char const	*p;

	p = s;
	while (*p != 0 && ft_strchr(accept, *p) == NULL)
		++p;
	if (*p == 0)
		p = NULL;
	return ((char *)p);
}
