/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 15:40:12 by bbrassar          #+#    #+#             */
/*   Updated: 2023/11/28 12:17:00 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"

#include <stdint.h>

__attribute__((always_inline))
inline static void	_memcmp64(void const **s1, void const **s2, size_t *len)
{
	uint64_t const	*b1 = *s1;
	uint64_t const	*b2 = *s2;

	while (*len > sizeof(uint64_t) && *b1 == *b2)
	{
		*len -= sizeof(uint64_t);
		++b1;
		++b2;
	}
	*s1 = b1;
	*s2 = b2;
}

__attribute__((always_inline))
inline static void	_memcmp32(void const **s1, void const **s2, size_t *len)
{
	uint32_t const	*b1 = *s1;
	uint32_t const	*b2 = *s2;

	while (*len > sizeof(uint32_t) && *b1 == *b2)
	{
		*len -= sizeof(uint32_t);
		++b1;
		++b2;
	}
	*s1 = b1;
	*s2 = b2;
}

__attribute__((always_inline))
inline static void	_memcmp16(void const **s1, void const **s2, size_t *len)
{
	uint16_t const	*b1 = *s1;
	uint16_t const	*b2 = *s2;

	while (*len > sizeof(uint16_t) && *b1 == *b2)
	{
		*len -= sizeof(uint16_t);
		++b1;
		++b2;
	}
	*s1 = b1;
	*s2 = b2;
}

__attribute__((always_inline))
inline static void	_memcmp8(void const **s1, void const **s2, size_t *len)
{
	uint8_t const	*b1 = *s1;
	uint8_t const	*b2 = *s2;

	while (*len > sizeof(uint8_t) && *b1 == *b2)
	{
		*len -= sizeof(uint8_t);
		++b1;
		++b2;
	}
	*s1 = b1;
	*s2 = b2;
}

int	ft_memcmp(void const *s1, void const *s2, size_t n)
{
	if (n == 0 || s1 == s2)
		return (0);
	_memcmp64(&s1, &s2, &n);
	_memcmp32(&s1, &s2, &n);
	_memcmp16(&s1, &s2, &n);
	_memcmp8(&s1, &s2, &n);
	return (*(uint8_t const *)s1 - *(uint8_t const *)s2);
}
