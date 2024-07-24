/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 10:39:06 by bbrassar          #+#    #+#             */
/*   Updated: 2024/07/24 10:56:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bswap.h"
#include "config.h"
#include "memory_map.h"
#include "options.h"
#include "version.h"

#include "ft_ext.h"
#include "libft/ft.h"

#include <elf.h>

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
# define ELFDATA_CURRENT ELFDATA2LSB
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
# define ELFDATA_CURRENT ELFDATA2MSB
#endif

#define FIX_BYTEORDER(ElfData, X) \
	(((ElfData) == ELFDATA_CURRENT) \
	? X \
	: GENERIC_SWAP(X))

#define ELF_MATCH(ElfClass, Case64, Case32) \
	(((ElfClass) == ELFCLASS64) ? (Case64) : (Case32))

#define ELF_SIZE(ElfClass, Type) \
	ELF_MATCH(elfclass, sizeof(Elf64_ ## Type), sizeof(Elf32_ ## Type))

#define ELF_GET(ElfClass, ElfData, Object, Param) \
	(ELF_MATCH(ElfClass, \
	(FIX_BYTEORDER(ElfData, (Object)->elf64.Param)), \
	(FIX_BYTEORDER(ElfData, (Object)->elf32.Param))))

struct symbol {
	uint64_t value;
	int has_value;
	char const *name;
	char type_char;
};

#define MAKE_ELF_UNION(Type) \
typedef union { \
	Elf32_ ## Type elf32; \
	Elf64_ ## Type elf64; \
} Elf_ ## Type;

MAKE_ELF_UNION(Ehdr);
MAKE_ELF_UNION(Shdr);
MAKE_ELF_UNION(Sym);

static char const COPYRIGHT_NOTICE[] =
	"\n"
	"\n"
	"Copyright (C) 2024 Benjamin Brassart\n"
	"\n"
	"This program is free and open source software.\n"
	"You may redistribute it under the terms of the MIT license.\n"
	"\n"
	"Report bugs and issues at https://github.com/bemjaminbrassart/ft_nm\n";

static int ft_nm(struct config *config, char const *files[], int n);

int main(int argc, char const *argv[])
{
	struct config config;

	ft_memcpy(&config, &DEFAULT_CONFIG, sizeof(struct config));

	switch (parse_options(&argc, argv, &config)) {
	case ParseConfigEarlyExit:
		return EXIT_SUCCESS;
	case ParseConfigFailure:
		return EXIT_FAILURE;
	case ParseConfigSuccess:
		break;
	}

	if (config.display_version) {
		write(STDOUT_FILENO, "ft_nm version ", 14);
		write(STDOUT_FILENO, VERSION_STRING, ft_strlen(VERSION_STRING));
		write(STDOUT_FILENO, COPYRIGHT_NOTICE, sizeof(COPYRIGHT_NOTICE) - 1);
		return EXIT_SUCCESS;
	}

	if (argc == 1) {
		argv[1] = "a.out";
		argc += 1;
	}

	if (argc > 2) {
		config.print_file_name = 1;
	}

	return ft_nm(&config, &argv[1], argc - 1);
}

static char _elf_section_type_char(Elf_Shdr const *section, char const *name, uint8_t elfclass, uint8_t elfdata)
{
	Elf64_Word const sh_type = ELF_GET(elfclass, elfdata, section, sh_type);
	Elf64_Xword const sh_flags = ELF_GET(elfclass, elfdata, section, sh_flags);

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

static char _elf_symbol_type_char(Elf_Sym const *symbol, void const *shdr, uint8_t elfclass, uint8_t elfdata)
{
	Elf64_Section const st_shndx = ELF_GET(elfclass, elfdata, symbol, st_shndx);
	uint8_t const st_info = ELF_GET(elfclass, elfdata, symbol, st_info);

	uint8_t const st_bind = ELF_MATCH(elfclass, ELF64_ST_BIND(st_info), ELF32_ST_BIND(st_info));
	uint8_t const st_type = ELF_MATCH(elfclass, ELF64_ST_TYPE(st_info), ELF32_ST_TYPE(st_info));

	if (st_shndx == SHN_ABS) {
		if (st_type == STT_FILE) {
			return 'a';
		} else {
			return 'A';
		}
	} else if (st_shndx == SHN_COMMON) {
		return 'C';
	} else if (st_shndx >= SHN_LORESERVE /* && st_shndx <= SHN_HIRESERVE */) {
		return '?';
	}

	uint8_t const *raw_shdr = (uint8_t const *)shdr;

	Elf_Shdr const *section = (Elf_Shdr const *)&raw_shdr[st_shndx * ELF_SIZE(elfclass, Shdr)];
	Elf64_Word const sh_type = ELF_GET(elfclass, elfdata, section, sh_type);
	Elf64_Xword const sh_flags = ELF_GET(elfclass, elfdata, section, sh_flags);

	char sym_char = '?';

	if (st_bind == STB_GNU_UNIQUE) {
		sym_char = 'u';
	} else if (st_shndx == SHN_UNDEF) {
		sym_char = 'U';

		if (st_bind == STB_WEAK) {
			if (st_type == STT_OBJECT) {
				sym_char = 'v';
			} else {
				sym_char = 'w';
			}
		}
	} else if (st_bind == STB_WEAK) {
		if (st_type == STT_OBJECT) {
			sym_char = 'V';
		} else {
			sym_char = 'W';
		}
	} else if (sh_type == SHT_PROGBITS && (sh_flags & (SHF_WRITE|SHF_ALLOC)) == SHF_ALLOC) {
		if ((sh_flags & SHF_EXECINSTR) == SHF_EXECINSTR) {
			sym_char = 'T';
		} else {
			sym_char = 'R';
		}
	} else if (sh_type == SHT_NOBITS && (sh_flags & (SHF_ALLOC|SHF_WRITE)) == (SHF_ALLOC|SHF_WRITE)) {
		sym_char = 'B';
	} else if (sh_type == SHT_PROGBITS && (sh_flags & (SHF_ALLOC|SHF_WRITE)) == (SHF_ALLOC|SHF_WRITE)) {
		sym_char = 'D';
	} else if (st_type == STT_FUNC) {
		sym_char = 'T';
	} else if (st_type == STT_OBJECT) {
		if (st_bind == STB_GLOBAL || (sh_flags & SHF_WRITE) == 0) {
			sym_char = 'R';
		} else {
			sym_char = 'd';
		}
	} else if (st_type == STT_NOTYPE && (sh_flags & (SHF_WRITE|SHF_EXECINSTR)) == 0) {
		if (sh_type == SHT_PROGBITS) {
			sym_char = 'r';
		} else {
			sym_char = 'n';
		}
	} else {
		sym_char = '?';
	}

	if (st_bind == STB_LOCAL) {
		sym_char = (char)ft_tolower(sym_char);
	}

	return sym_char;
}

static int _compare_symbol(void const *p1, void const *p2)
{
	struct symbol const *sym1 = p1;
	struct symbol const *sym2 = p2;
	char const *s1 = sym1->name;
	char const *s2 = sym2->name;

	if (s1 == s2) {
		return 0;
	}

	if (s1 == NULL) {
		return 1;
	} else if (s2 == NULL) {
		return -1;
	}

	while (1) {
		while (*s1 == '_' || *s1 == '.') {
			s1 += 1;
		}

		while (*s2 == '_' || *s2 == '.') {
			s2 += 1;
		}

		if (*s1 != '\0' && ft_toupper(*s1) == ft_toupper(*s2)) {
			s1 += 1;
			s2 += 1;
		} else {
			break;
		}
	}

	// without '_' s1 and s2 are the same (case-insensitive)
	// perform case-sensitive ascii string comparison
	if (*s1 == '\0' && *s2 == '\0') {
		return ft_strcmp(sym1->name, sym2->name);
	}

	return ft_toupper(*s1) - ft_toupper(*s2);
}

static void _fill_offbuf(char buf[], size_t n, uint64_t value)
{
	static char const base[16] = "0123456789abcdef";

	ft_memset(buf, '0', n);
	while (n > 0 && value > 0) {
		buf[n - 1] = base[value % sizeof(base)];
		value /= sizeof(base);
		n -= 1;
	}
}

// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!
// TODO add bounds checking EVERYWHERE!!

static int _ft_nm_elf(struct config const *config, struct memory_map *mm, Elf_Ehdr const *ehdr, char const *file)
{
	bool extern_only = false;
	bool undefined_only = false;
	bool debug_symbols = false;

	uint8_t const elfclass = ehdr->elf32.e_ident[EI_CLASS];
	uint8_t const elfdata = ehdr->elf32.e_ident[EI_DATA];

	if (config->debug_symbols) {
		debug_symbols = true;
	}

	if (config->extern_only) {
		undefined_only = false;
		extern_only = true;
		debug_symbols = false;
	}

	if (config->undefined_only) {
		undefined_only = true;
		extern_only = false;
		debug_symbols = false;
	}

	uint8_t const *raw_map = mm->map;

	uint8_t const *raw_shdr = &raw_map[ELF_GET(elfclass, elfdata, ehdr, e_shoff)];
	Elf_Shdr const *symtab_shdr = NULL;
	Elf_Shdr const *strtab_shdr = NULL;
	char const *string_table = NULL;
	Elf64_Word string_table_length = 0;

	// TODO check string_table_length
	(void)string_table_length;

	Elf64_Xword sym_count = 0;
	struct symbol *symbols = NULL;

	Elf64_Half const e_shsum = ELF_GET(elfclass, elfdata, ehdr, e_shnum);

	for (Elf64_Half i = 0; i < e_shsum; i += 1) {
		Elf_Shdr const *section = (Elf_Shdr const *)&raw_shdr[i * ELF_SIZE(elfclass, Shdr)];
		Elf64_Word const sh_type = ELF_GET(elfclass, elfdata, section, sh_type);

		if (sh_type != SHT_SYMTAB) {
			continue;
		}

		Elf64_Word const sh_link = ELF_GET(elfclass, elfdata, section, sh_link);

		symtab_shdr = section;
		strtab_shdr = (Elf_Shdr const *)&raw_shdr[sh_link * ELF_SIZE(elfclass, Shdr)];

		Elf64_Xword sh_size = ELF_GET(elfclass, elfdata, symtab_shdr, sh_size);
		Elf64_Xword sh_entsize = ELF_GET(elfclass, elfdata, symtab_shdr, sh_entsize);

		sym_count = sh_size / sh_entsize;
		string_table = (char const *)&raw_map[ELF_GET(elfclass, elfdata, strtab_shdr, sh_offset)];
		string_table_length = (Elf64_Word)ELF_GET(elfclass, elfdata, strtab_shdr, sh_size);
		break;
	}

	symbols = ft_calloc((size_t)sym_count, sizeof(*symbols));
	if (symbols == NULL) {
		return EXIT_FAILURE;
	}

	uint8_t const *raw_symbol_table = &raw_map[ELF_GET(elfclass, elfdata, symtab_shdr, sh_offset)];
	size_t sym_i = 0;

	for (Elf64_Xword j = 0; j < sym_count; j += 1) {
		Elf_Sym const *symbol = (Elf_Sym const *)&raw_symbol_table[j * ELF_SIZE(elfclass, Sym)];
		char const *symbol_name;
		char type_char;

		uint8_t const st_info = ELF_GET(elfclass, elfdata, symbol, st_info);
		uint8_t const st_type = ELF_MATCH(elfclass, ELF64_ST_TYPE(st_info), ELF32_ST_TYPE(st_info));

		if (st_type == STT_SECTION) {
			if (!debug_symbols) {
				continue;
			}

			Elf_Shdr const *section_symbol = (Elf_Shdr const *)&raw_shdr[ELF_GET(elfclass, elfdata, symbol, st_shndx) * ELF_SIZE(elfclass, Shdr)];
			Elf_Shdr const *section_strtab = (Elf_Shdr const *)&raw_shdr[ELF_GET(elfclass, elfdata, ehdr, e_shstrndx) * ELF_SIZE(elfclass, Shdr)];
			char const *section_string_table = (char const *)&raw_map[ELF_GET(elfclass, elfdata, section_strtab, sh_offset)];

			symbol_name = &section_string_table[ELF_GET(elfclass, elfdata, section_symbol, sh_name)];
			type_char = _elf_section_type_char(section_symbol, symbol_name, elfclass, elfdata);
		} else {
			symbol_name = &string_table[ELF_GET(elfclass, elfdata, symbol, st_name)];
			type_char = _elf_symbol_type_char(symbol, raw_shdr, elfclass, elfdata);
		}

		if (type_char != 'a' && symbol_name[0] == '\0') {
			continue;
		}

		if (!debug_symbols && type_char == 'a') {
			continue;
		}

		if (undefined_only && type_char != 'U' && type_char != 'w') {
			continue;
		}

		if (extern_only) {
			switch (type_char){
			case 'B':
			case 'D':
			case 'R':
			case 'T':
			case 'U':
			case 'w':
			case 'W':
				break;

			default:
				continue;
			}
		}

		symbols[sym_i].name = symbol_name;
		symbols[sym_i].type_char = type_char;

		switch (symbols[sym_i].type_char) {
		case 'a':
		case 'A':
		case 'b':
		case 'B':
		case 'd':
		case 'D':
		case 'n':
		case 'N':
		case 'r':
		case 'R':
		case 't':
		case 'T':
		case 'V':
		case 'W':
			symbols[sym_i].has_value = 1;
			symbols[sym_i].value = ELF_GET(elfclass, elfdata, symbol, st_value);
			break;

		default:
			symbols[sym_i].has_value = 0;
			break;
		}

		sym_i += 1;
	}

	if (!config->no_sort) {
		ft_qsort(symbols, sym_i, sizeof(*symbols), _compare_symbol);
	}

	size_t sizeof_offbuf = ELF_MATCH(elfclass, 16, 8);
	char *offbuf;

	offbuf = malloc(sizeof(*offbuf) * sizeof_offbuf);
	if (offbuf == NULL) {
		free(symbols);
		return EXIT_FAILURE;
	}

	struct symbol *symbol;

	if (config->print_file_name) {
		write(STDOUT_FILENO, "\n", 1);
		write(STDOUT_FILENO, file, ft_strlen(file));
		write(STDOUT_FILENO, ":\n", 2);
	}

	size_t printed_symbols = 0;

	for (size_t i = 0; i < sym_i; i += 1) {
		if (!config->no_sort && config->reverse_sort) {
			symbol = &symbols[sym_i - i - 1];
		} else {
			symbol = &symbols[i];
		}

		if (symbol->name == NULL) {
			continue;
		}

		if (symbol->has_value) {
			_fill_offbuf(offbuf, sizeof_offbuf, symbol->value);
		} else {
			ft_memset(offbuf, ' ', sizeof_offbuf);
		}

		write(STDOUT_FILENO, offbuf, sizeof_offbuf);
		write(STDOUT_FILENO, " ", 1);
		write(STDOUT_FILENO, &symbol->type_char, 1);
		write(STDOUT_FILENO, " ", 1);
		write(STDOUT_FILENO, symbol->name, ft_strlen(symbol->name));
		write(STDOUT_FILENO, "\n", 1);

		printed_symbols += 1;
	}

	if (printed_symbols == 0) {
		write(STDERR_FILENO, "ft_nm: ", 8);
		write(STDERR_FILENO, file, ft_strlen(file));
		write(STDERR_FILENO, ": no symbols\n", 13);
	}

	free(offbuf);
	free(symbols);

	(void)mm;
	(void)ehdr;
	(void)file;
	return EXIT_SUCCESS;
}

static int ft_nm_elf(struct config const *config, struct memory_map *mm, Elf32_Ehdr const *ehdr, char const *file)
{
	switch (ehdr->e_ident[EI_CLASS]) {
	case ELFCLASS32:
		// return ft_nm_elf32(config, mm, ehdr, file);

	case ELFCLASS64: {
		return _ft_nm_elf(config, mm, mm->map, file);
	}

	default:
		return EXIT_FAILURE;
	}
}

static int ft_nm_map(struct config const *config, void const *map, size_t map_size, char const *file)
{
	(void)file;

	struct memory_map root_map = {
		.map = map,
		.map_size = map_size,
	};

	Elf32_Ehdr const *ehdr = mm_read(&root_map, 0, sizeof(*ehdr));

	if (ehdr == NULL) {
		return EXIT_FAILURE;
	}

	if (ft_memcmp(&ehdr->e_ident[EI_MAG0], ELFMAG, SELFMAG) == 0) {
		return ft_nm_elf(config, &root_map, ehdr, file);
	} else {
		return EXIT_FAILURE;
	}
}

static int ft_nm_file(struct config const *config, char const *file)
{
	int result = EXIT_FAILURE;

	int err;
	char const *err_msg;

	int fd = -1;
	struct stat st;
	size_t map_size = 0;
	void *map = MAP_FAILED;

	do {
		/* O_NONBLOCK to avoid blocking on named pipes (FIFOs) */
		fd = open(file, O_RDONLY|O_NONBLOCK);
		if (fd == -1) {
			err = errno;
			err_msg = strerror(err);

			write(STDERR_FILENO, "ft_nm: cannot open ", 19);
			write(STDERR_FILENO, file, ft_strlen(file));
			write(STDERR_FILENO, ": ", 2);
			write(STDERR_FILENO, err_msg, ft_strlen(err_msg));
			write(STDERR_FILENO, "\n", 1);

			break;
		}

		if (fstat(fd, &st) == -1) {
			err = errno;
			err_msg = strerror(err);

			write(STDERR_FILENO, "ft_nm: cannot stat ", 19);
			write(STDERR_FILENO, file, ft_strlen(file));
			write(STDERR_FILENO, ": ", 2);
			write(STDERR_FILENO, err_msg, ft_strlen(err_msg));
			write(STDERR_FILENO, "\n", 1);
			break;
		}

		if (S_ISDIR(st.st_mode)) {
			err_msg = strerror(EISDIR);

			write(STDERR_FILENO, "ft_nm: cannot map ", 18);
			write(STDERR_FILENO, file, ft_strlen(file));
			write(STDERR_FILENO, ": ", 2);
			write(STDERR_FILENO, err_msg, ft_strlen(err_msg));
			write(STDERR_FILENO, "\n", 1);
			break;
		}

		map_size = (size_t)st.st_size;

		map = mmap(NULL, map_size, PROT_READ, MAP_PRIVATE, fd, 0);
		if (map == MAP_FAILED) {
			err = errno;
			err_msg = strerror(err);

			write(STDERR_FILENO, "ft_nm: cannot map ", 18);
			write(STDERR_FILENO, file, ft_strlen(file));
			write(STDERR_FILENO, ": ", 2);
			write(STDERR_FILENO, err_msg, ft_strlen(err_msg));
			write(STDERR_FILENO, "\n", 1);
			break;
		}

		close(fd);
		fd = -1;

		result = ft_nm_map(config, map, map_size, file);
	} while (false);

	if (map != MAP_FAILED) {
		if (munmap(map, map_size) == -1) {
			err = errno;
			err_msg = strerror(err);

			write(STDERR_FILENO, "ft_nm: munmap: ", 15);
			write(STDERR_FILENO, err_msg, ft_strlen(err_msg));
			write(STDERR_FILENO, "\n", 1);
		}
	}

	if (fd != -1) {
		close(fd);
	}

	return result;
}

static int ft_nm(struct config *config, char const *files[], int n)
{
	/**
	 * yes, this is the behaviour of GNU nm.
	 * yes, the program exits with 0 if there is 256 errors.
	 * yes, it is dumb as a box of rocks.
	 * yes, i laughed very hard when i found this.
	 */
	int result;

	result = 0;
	for (int i = 0; i < n; i += 1) {
		if (ft_nm_file(config, files[i]) != EXIT_SUCCESS) {
			result += 1;
		}
	}
	return result;
}
