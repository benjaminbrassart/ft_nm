/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bswap.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 08:01:28 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 08:18:15 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft_byteswap.h"
#include <stdint.h>

uint16_t	ft_bswap_16(uint16_t bytes)
{
	return ((bytes >> 8) | (bytes << 8));
}

uint32_t	ft_bswap_32(uint32_t bytes)
{
	return (((uint32_t)ft_bswap_16(bytes)) << 16 | ft_bswap_16(bytes >> 16));
}

uint64_t	ft_bswap_64(uint64_t bytes)
{
	return (((uint64_t)ft_bswap_32(bytes)) << 32 | ft_bswap_32(bytes >> 32));
}
