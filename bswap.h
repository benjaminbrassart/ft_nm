/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bswap.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 22:06:49 by bbrassar          #+#    #+#             */
/*   Updated: 2024/07/23 22:48:18 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdint.h>
#include <sys/cdefs.h>

uint16_t swap_u16(uint16_t n);
int16_t  swap_i16(int16_t n);

uint32_t swap_u32(uint32_t n);
int32_t  swap_i32(int32_t n);

uint64_t swap_u64(uint64_t n);
int64_t  swap_i64(int64_t n);

#define GENERIC_SWAP(X) \
	(_Generic((X), \
		uint8_t:  X,           int8_t:  X, \
		uint16_t: swap_u16(X), int16_t: swap_i16(X), \
		uint32_t: swap_u32(X), int32_t: swap_i32(X), \
		uint64_t: swap_u64(X), int64_t: swap_i64(X), \
		default: ({}) \
	))
