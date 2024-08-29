/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/23 12:12:03 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:57:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"

char	*ft_strtrim(char const *s, char const *set)
{
	size_t	i;

	if (!(s && set))
		return (NULL);
	while (*s && ft_strchr(set, *s))
		++s;
	i = ft_strlen(s);
	if (i-- == 0)
		return (ft_strdup(""));
	while (ft_strchr(set, s[i]))
		--i;
	return (ft_substr(s, 0, i + 1));
}
