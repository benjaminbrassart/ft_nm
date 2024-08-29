/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/22 10:04:05 by bbrassar          #+#    #+#             */
/*   Updated: 2023/11/28 11:44:39 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"

#include <stdint.h>

__attribute__((always_inline))
inline static void	_memmove64(void **dst, void const **src, size_t *len)
{
	uint64_t		*d;
	uint64_t const	*s;

	d = *dst;
	s = *src;
	while (*len >= sizeof(uint64_t))
	{
		*--d = *--s;
		*len -= sizeof(uint64_t);
	}
	*dst = d;
	*src = s;
}

__attribute__((always_inline))
inline static void	_memmove32(void **dst, void const **src, size_t *len)
{
	uint32_t		*d;
	uint32_t const	*s;

	d = *dst;
	s = *src;
	while (*len >= sizeof(uint32_t))
	{
		*--d = *--s;
		*len -= sizeof(uint32_t);
	}
	*dst = d;
	*src = s;
}

__attribute__((always_inline))
inline static void	_memmove16(void **dst, void const **src, size_t *len)
{
	uint16_t		*d;
	uint16_t const	*s;

	d = *dst;
	s = *src;
	while (*len >= sizeof(uint16_t))
	{
		*--d = *--s;
		*len -= sizeof(uint16_t);
	}
	*dst = d;
	*src = s;
}

__attribute__((always_inline))
inline static void	_memmove8(void **dst, void const **src, size_t *len)
{
	uint8_t			*d;
	uint8_t const	*s;

	d = *dst;
	s = *src;
	while (*len >= sizeof(uint8_t))
	{
		*--d = *--s;
		*len -= sizeof(uint8_t);
	}
	*dst = d;
	*src = s;
}

void	*ft_memmove(void *dst, void const *src, size_t len)
{
	void		*d;
	void const	*s;

	if (len == 0 || dst == src)
		return (dst);
	if (src > dst)
		return (ft_memcpy(dst, src, len));
	d = ((uint8_t *)dst)+ len;
	s = ((uint8_t const *)src) + len;
	_memmove64(&d, &s, &len);
	_memmove32(&d, &s, &len);
	_memmove16(&d, &s, &len);
	_memmove8(&d, &s, &len);
	return (dst);
}
