/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 15:27:42 by bbrassar          #+#    #+#             */
/*   Updated: 2023/11/28 11:24:00 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"

#include <stdint.h>

__attribute__((always_inline))
inline static void	_memset64(void **b, uint64_t c, size_t *len)
{
	uint64_t	*s;

	s = *b;
	while (*len >= sizeof(uint64_t))
	{
		*s++ = c;
		*len -= sizeof(uint64_t);
	}
	*b = s;
}

__attribute__((always_inline))
inline static void	_memset32(void **b, uint32_t c, size_t *len)
{
	uint32_t	*s;

	s = *b;
	while (*len >= sizeof(uint32_t))
	{
		*s++ = c;
		*len -= sizeof(uint32_t);
	}
	*b = s;
}

__attribute__((always_inline))
inline static void	_memset16(void **b, uint16_t c, size_t *len)
{
	uint16_t	*s;

	s = *b;
	while (*len >= sizeof(uint16_t))
	{
		*s++ = c;
		*len -= sizeof(uint16_t);
	}
	*b = s;
}

__attribute__((always_inline))
inline static void	_memset8(void **b, uint8_t c, size_t *len)
{
	uint8_t	*s;

	s = *b;
	while (*len >= sizeof(uint8_t))
	{
		*s++ = c;
		*len -= sizeof(uint8_t);
	}
	*b = s;
}

void	*ft_memset(void *b, int c, size_t len)
{
	uint64_t	v;
	void		*s;

	if (len != 0)
	{
		v = c & 0xff;
		v |= v << 8 | v << 16 | v << 24 | v << 32 | v << 40 | v << 48 | v << 56;
		s = b;
		_memset64(&s, v, &len);
		_memset32(&s, v, &len);
		_memset16(&s, v, &len);
		_memset8(&s, v, &len);
	}
	return (b);
}
