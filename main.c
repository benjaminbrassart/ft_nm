/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 10:39:06 by bbrassar          #+#    #+#             */
/*   Updated: 2024/07/23 01:29:22 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

struct symbol {
	uint64_t offset;
	int has_address;
	char const *name;
	char type_char;
};

char const COPYRIGHT_NOTICE[] =
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

static int ft_nm_elf32(struct config const *config, struct memory_map *mm, Elf32_Ehdr const *ehdr, char const *file)
{
	(void)config;
	(void)mm;
	(void)ehdr;
	(void)file;
	return EXIT_SUCCESS;
}

static char _elf64_section_type_char(Elf64_Shdr const *section, char const *name)
{
	Elf64_Word const sh_type = section->sh_type;
	Elf64_Xword const sh_flags = section->sh_flags;

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

static char _elf64_symbol_type_char(Elf64_Sym const *symbol, Elf64_Shdr const *shdr)
{
	Elf64_Section const st_shndx = symbol->st_shndx;
	unsigned char const st_bind = ELF64_ST_BIND(symbol->st_info);
	unsigned char const st_type = ELF64_ST_TYPE(symbol->st_info);

	if (st_shndx == SHN_ABS) {
		if (st_type == STT_FILE) {
			return 'a';
		} else {
			return 'A';
		}
	} else if (st_shndx == SHN_COMMON) {
		return 'C';
	} else if (st_shndx >= SHN_LORESERVE && st_shndx <= SHN_HIRESERVE) {
		return '?';
	}

	Elf64_Shdr const *section = &shdr[st_shndx];
	Elf64_Word const sh_type = section->sh_type;
	Elf64_Xword const sh_flags = section->sh_flags;

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
	} else if (sh_type == SHT_PROGBITS && sh_flags == SHF_ALLOC) {
		if ((sh_flags & SHF_EXECINSTR) == SHF_EXECINSTR) {
			sym_char = 'T';
		} else {
			sym_char = 'R';
		}
	} else if (sh_type == SHT_NOBITS && sh_flags == (SHF_ALLOC|SHF_WRITE)) {
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
	} else if (st_type == STT_SECTION) {
		sym_char = '?';
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

static void _remove_duplicate_and_unversioned_symbols(struct symbol *symbols, size_t n)
{
	for (size_t i = 0; i < n; i += 1) {
		if (symbols[i].name == NULL) {
			continue;
		}

		for (size_t j = 0; j < i; j += 1) {
			if (symbols[j].name != NULL && strcmp(symbols[i].name, symbols[j].name) == 0) {
				symbols[j].name = NULL;
			}
		}

		char const *at = ft_strchr(symbols[i].name, '@');

		if (at == NULL) {
			continue;
		}

		size_t label_length = (size_t)(at - symbols[i].name);

		for (size_t j = 0; j < n; j += 1) {
			if (i == j || symbols[j].name == NULL) {
				continue;
			}

			if (ft_strncmp(symbols[i].name, symbols[j].name, label_length) == 0 && symbols[j].name[label_length] == '\0') {
				symbols[j].name = NULL;
			}
		}
	}
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

static int ft_nm_elf64(struct config const *config, struct memory_map *mm, Elf64_Ehdr const *ehdr, char const *file)
{
	bool extern_only = false;
	bool undefined_only = false;
	bool debug_symbols = false;

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

	Elf64_Shdr const *shdr = (Elf64_Shdr const *)((unsigned char const *)mm->map + ehdr->e_shoff);

	Elf64_Shdr const *section_string_table_shdr = &shdr[ehdr->e_shstrndx];
	char const *section_header_string_table = (char const *)((unsigned char const *)mm->map + section_string_table_shdr->sh_offset);

	char const *symbol_string_table = NULL;
	// char const *dynamic_string_table = NULL;

	for (Elf64_Half i = 0; i < ehdr->e_shnum; i += 1) {
		Elf64_Shdr const *section = &shdr[i];
		char const *section_name = &section_header_string_table[section->sh_name];

		if (section->sh_type == SHT_STRTAB) {
			if (ft_strcmp(section_name, ".strtab") == 0) {
				symbol_string_table = (char const *)((unsigned char const *)mm->map + section->sh_offset);
			// } else if (ft_strcmp(section_name, ".dynstr") == 0) {
			// 	dynamic_string_table = (char const *)((unsigned char const *)mm->map + section->sh_offset);
			}
		}
	}

	size_t total_symbols = 0;
	struct symbol *symbols = NULL;

	for (Elf64_Half i = 0; i < ehdr->e_shnum; i += 1) {
		Elf64_Shdr const *section = &shdr[i];

		switch (section->sh_type) {
		case SHT_DYNSYM:
		case SHT_SYMTAB:
			break;

		default:
			continue;
		}

		Elf64_Xword sym_count = section->sh_size / section->sh_entsize;

		total_symbols += sym_count;
	}

	symbols = ft_calloc(total_symbols, sizeof(*symbols));
	if (symbols == NULL) {
		return EXIT_FAILURE;
	}

	size_t sym_i = 0;

	for (Elf64_Half i = 0; i < ehdr->e_shnum; i += 1) {
		Elf64_Shdr const *section = &shdr[i];
		char const *string_table;

		switch (section->sh_type) {
		// case SHT_DYNSYM:
		// 	string_table = dynamic_string_table;
		// 	break;

		case SHT_SYMTAB:
			string_table = symbol_string_table;
			break;

		default:
			continue;
		}

		Elf64_Xword sym_count = section->sh_size / section->sh_entsize;
		Elf64_Sym const *symbol_table = (Elf64_Sym const *)((unsigned char const *)mm->map + section->sh_offset);

		for (Elf64_Xword j = 0; j < sym_count; j += 1) {
			Elf64_Sym const *symbol = &symbol_table[j];
			char const *symbol_name;
			char type_char;

			if (ELF64_ST_TYPE(symbol->st_info) == STT_SECTION) {
				Elf64_Shdr const *section_symbol = &shdr[symbol->st_shndx];

				symbol_name = &string_table[section_symbol->sh_name];
				type_char = _elf64_section_type_char(section_symbol, symbol_name);
			} else {
				symbol_name = &string_table[symbol->st_name];
				type_char = _elf64_symbol_type_char(symbol, shdr);
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
			case 'N':
			case 'r':
			case 'R':
			case 't':
			case 'T':
			case 'W':
				symbols[sym_i].has_address = 1;
				symbols[sym_i].offset = symbol->st_value;
				break;

			default:
				symbols[sym_i].has_address = 0;
				break;
			}

			sym_i += 1;
		}
	}

	if (!config->no_sort) {
		ft_qsort(symbols, sym_i, sizeof(*symbols), _compare_symbol);
	}

	_remove_duplicate_and_unversioned_symbols(symbols, sym_i);

	char offbuf[16];
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

		if (symbol->has_address) {
			_fill_offbuf(offbuf, sizeof(offbuf), symbol->offset);
		} else {
			ft_memset(offbuf, ' ', sizeof(offbuf));
		}

		write(STDOUT_FILENO, offbuf, sizeof(offbuf));
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
		return ft_nm_elf32(config, mm, ehdr, file);

	case ELFCLASS64: {
		Elf64_Ehdr const *ehdr64;

		ehdr64 = mm_read(mm, NULL, sizeof(*ehdr64));
		if (ehdr64 == NULL) {
			return EXIT_FAILURE;
		}

		return ft_nm_elf64(config, mm, ehdr64, file);
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

	Elf32_Ehdr const *ehdr;

	ehdr = mm_read(&root_map, NULL, sizeof(*ehdr));
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
