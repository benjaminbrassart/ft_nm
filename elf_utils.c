/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 15:42:04 by bbrassar          #+#    #+#             */
/*   Updated: 2024/08/28 15:43:53 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "elf_utils.h"

#include "libft/ft.h"

#include <stdbool.h>

char _elf_section_type_char(Elf_Shdr const *section, char const *name,
				   uint8_t elfclass, uint8_t elfdata)
{
	Elf64_Word const sh_type = ELF_GET(elfclass, elfdata, section, sh_type);
	Elf64_Xword const sh_flags =
		ELF_GET(elfclass, elfdata, section, sh_flags);

	char sym_char = '?';

	if (sh_type == SHT_PROGBITS) {
		if (ft_strncmp(name, ".debug_", 7) == 0) {
			sym_char = 'N';
		} else if ((sh_flags & SHF_EXECINSTR) == SHF_EXECINSTR) {
			sym_char = 't';
		} else if ((sh_flags & SHF_WRITE) == SHF_WRITE) {
			sym_char = 'd';
		} else {
			sym_char = 'r';
		}
	} else if (sh_type == SHT_NOBITS) {
		sym_char = 'b';
	}

	return sym_char;
}

char _elf_symbol_type_char(Elf_Sym const *symbol, void const *shdr,
				  uint8_t elfclass, uint8_t elfdata)
{
	Elf64_Section const st_shndx =
		ELF_GET(elfclass, elfdata, symbol, st_shndx);
	uint8_t const st_info = ELF_GET(elfclass, elfdata, symbol, st_info);

	uint8_t const st_bind = ELF_MATCH(elfclass, ELF64_ST_BIND(st_info),
					  ELF32_ST_BIND(st_info));
	uint8_t const st_type = ELF_MATCH(elfclass, ELF64_ST_TYPE(st_info),
					  ELF32_ST_TYPE(st_info));

	if (st_shndx == SHN_ABS) {
		if (st_type == STT_FILE) {
			return 'a';
		} else {
			return 'A';
		}
	} else if (st_shndx == SHN_COMMON) {
		return 'C';
	} else if (st_shndx == SHN_UNDEF) {
		if (st_bind == STB_WEAK) {
			if (st_type == STT_OBJECT) {
				return 'v';
			} else {
				return 'w';
			}
		} else {
			return 'U';
		}
	} else if (st_shndx >=
		   SHN_LORESERVE /* && st_shndx <= SHN_HIRESERVE */) {
		return '?';
	}

	if (st_bind == STB_GNU_UNIQUE) {
		return 'u';
	}

	uint8_t const *raw_shdr = (uint8_t const *)shdr;

	Elf_Shdr const *section =
		(Elf_Shdr const
			 *)&raw_shdr[st_shndx * ELF_SIZE(elfclass, Elf_Shdr)];
	Elf64_Word const sh_type = ELF_GET(elfclass, elfdata, section, sh_type);
	Elf64_Xword const sh_flags =
		ELF_GET(elfclass, elfdata, section, sh_flags);

	char sym_char = '?';

	if (st_bind == STB_GNU_UNIQUE) {
		sym_char = 'u';
	} else if (st_bind == STB_WEAK) {
		if (st_type == STT_OBJECT) {
			sym_char = 'V';
		} else {
			sym_char = 'W';
		}
	} else if (sh_type == SHT_PROGBITS &&
		   (sh_flags & (SHF_WRITE | SHF_ALLOC)) == SHF_ALLOC) {
		if ((sh_flags & SHF_EXECINSTR) == SHF_EXECINSTR) {
			sym_char = 'T';
		} else {
			sym_char = 'R';
		}
	} else if (sh_type == SHT_NOBITS &&
		   (sh_flags & (SHF_ALLOC | SHF_WRITE)) ==
			   (SHF_ALLOC | SHF_WRITE)) {
		sym_char = 'B';
	} else if (sh_type == SHT_PROGBITS &&
		   (sh_flags & (SHF_ALLOC | SHF_WRITE)) ==
			   (SHF_ALLOC | SHF_WRITE)) {
		sym_char = 'D';
	} else if (st_type == STT_FUNC) {
		sym_char = 'T';
	} else if (st_type == STT_OBJECT) {
		if (st_bind == STB_GLOBAL || (sh_flags & SHF_WRITE) == 0) {
			sym_char = 'R';
		} else {
			sym_char = 'd';
		}
	} else if (st_type == STT_NOTYPE &&
		   (sh_flags & (SHF_WRITE | SHF_EXECINSTR)) == 0) {
		if (sh_type == SHT_PROGBITS) {
			sym_char = 'r';
		} else {
			sym_char = 'n';
		}
	}

	if (st_bind == STB_LOCAL) {
		sym_char = (char)ft_tolower(sym_char);
	}

	return sym_char;
}

char _symbol_name_at(struct symbol const *sym, size_t index)
{
	if (index >= sym->name_length) {
		return '\0';
	}

	return sym->name[index];
}

int _compare_symbol(void const *p1, void const *p2)
{
	struct symbol const *sym1 = p1;
	struct symbol const *sym2 = p2;
	size_t i1 = 0;
	size_t i2 = 0;
	char c1;
	char c2;

	if (sym1->name == sym2->name) {
		return 0;
	}

	if (sym1->name == NULL) {
		return 1;
	} else if (sym2->name == NULL) {
		return -1;
	}

	while (true) {
		while (true) {
			c1 = _symbol_name_at(sym1, i1);
			if (ft_isalnum(c1) || c1 == '\0') {
				break;
			}
			i1 += 1;
		}

		while (true) {
			c2 = _symbol_name_at(sym2, i2);
			if (ft_isalnum(c2) || c2 == '\0') {
				break;
			}
			i2 += 1;
		}

		if (c1 == '\0' || c2 == '\0' ||
		    ft_toupper(c1) != ft_toupper(c2)) {
			break;
		}

		i1 += 1;
		i2 += 1;
	}

	// taking into account only alphanumeric ascii characters, s1 == s2.
	//
	// perform 'normal' ascii comparison, so that, for example,
	// '__data_start' appears before 'data_start'.
	if (c1 == '\0' && c2 == '\0') {
		size_t i_min;

		if (sym1->name_length > sym2->name_length) {
			i_min = sym2->name_length;
		} else {
			i_min = sym1->name_length;
		}

		return ft_strncmp(sym1->name, sym2->name, i_min);
	}

	return ft_toupper(c1) - ft_toupper(c2);
}
