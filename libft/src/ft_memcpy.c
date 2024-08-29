/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 15:31:32 by bbrassar          #+#    #+#             */
/*   Updated: 2023/11/28 11:20:23 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"

#include <stdint.h>

__attribute__((always_inline))
inline static void	_memcpy64(void **dst, void const **src, size_t *len)
{
	uint64_t const	*s;
	uint64_t		*d;

	s = *src;
	d = *dst;
	while (*len >= sizeof(uint64_t))
	{
		*d++ = *s++;
		*len -= sizeof(uint64_t);
	}
	*dst = d;
	*src = s;
}

__attribute__((always_inline))
inline static void	_memcpy32(void **dst, void const **src, size_t *len)
{
	uint32_t const	*s;
	uint32_t		*d;

	s = *src;
	d = *dst;
	while (*len >= sizeof(uint32_t))
	{
		*d++ = *s++;
		*len -= sizeof(uint32_t);
	}
	*dst = d;
	*src = s;
}

__attribute__((always_inline))
inline static void	_memcpy16(void **dst, void const **src, size_t *len)
{
	uint16_t const	*s;
	uint16_t		*d;

	s = *src;
	d = *dst;
	while (*len >= sizeof(uint16_t))
	{
		*d++ = *s++;
		*len -= sizeof(uint16_t);
	}
	*dst = d;
	*src = s;
}

__attribute__((always_inline))
inline static void	_memcpy8(void **dst, void const **src, size_t *len)
{
	uint8_t const	*s;
	uint8_t			*d;

	s = *src;
	d = *dst;
	while (*len >= sizeof(uint8_t))
	{
		*d++ = *s++;
		*len -= sizeof(uint8_t);
	}
	*dst = d;
	*src = s;
}

void	*ft_memcpy(void *dst, void const *src, size_t len)
{
	void	*d;

	if (len == 0 || dst == src)
		return (dst);
	d = dst;
	_memcpy64(&dst, &src, &len);
	_memcpy32(&dst, &src, &len);
	_memcpy16(&dst, &src, &len);
	_memcpy8(&dst, &src, &len);
	return (d);
}
