/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf_utils.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 15:37:38 by bbrassar          #+#    #+#             */
/*   Updated: 2024/08/28 15:43:55 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "bswap.h"

#include <elf.h>

#include <stddef.h>
#include <stdint.h>

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define ELFDATA_CURRENT ELFDATA2LSB
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define ELFDATA_CURRENT ELFDATA2MSB
#endif

#define FIX_BYTEORDER(ElfData, X) \
	(((ElfData) == ELFDATA_CURRENT) ? (X) : (GENERIC_SWAP(X)))

#define sizeof_field(Struct, Field) (sizeof(((Struct const *)NULL)->Field))

#define ELF_MATCH(ElfClass, Case64, Case32) \
	(((ElfClass) == ELFCLASS64) ? (Case64) : (Case32))

#define ELF_SIZE(ElfClass, Type)                         \
	ELF_MATCH((ElfClass), sizeof_field(Type, elf64), \
		  sizeof_field(Type, elf32))

#define ELF_GET(ElfClass, ElfData, Object, Param)                   \
	(ELF_MATCH((ElfClass),                                      \
		   (FIX_BYTEORDER(ElfData, (Object)->elf64.Param)), \
		   (FIX_BYTEORDER(ElfData, (Object)->elf32.Param))))

struct symbol {
	uint64_t value;
	int has_value;
	char const *name;
	size_t name_length;
	char type_char;
};

#define MAKE_ELF_UNION(Type)        \
	typedef union {             \
		Elf32_##Type elf32; \
		Elf64_##Type elf64; \
	} Elf_##Type;

MAKE_ELF_UNION(Ehdr);
MAKE_ELF_UNION(Shdr);
MAKE_ELF_UNION(Sym);

char _elf_section_type_char(Elf_Shdr const *section, char const *name,
				   uint8_t elfclass, uint8_t elfdata);

char _elf_symbol_type_char(Elf_Sym const *symbol, void const *shdr,
				  uint8_t elfclass, uint8_t elfdata);

char _symbol_name_at(struct symbol const *sym, size_t index);

int _compare_symbol(void const *p1, void const *p2);
