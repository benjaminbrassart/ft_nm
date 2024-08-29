/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/22 11:12:42 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"
#include <stdlib.h>

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	lengths[2];
	char	*new_s;

	ft_memset(lengths, 0, sizeof (lengths));
	if (s1 != NULL)
		lengths[0] = ft_strlen(s1);
	if (s2 != NULL)
		lengths[1] = ft_strlen(s2);
	new_s = malloc((lengths[0] + lengths[1] + 1) * sizeof (*new_s));
	if (new_s != NULL)
	{
		ft_memmove(new_s, s1, lengths[0]);
		ft_memmove(new_s + lengths[0], new_s, lengths[1]);
		new_s[lengths[0] + lengths[1]] = 0;
	}
	return (new_s);
}
