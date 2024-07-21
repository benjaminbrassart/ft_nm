/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 10:39:06 by bbrassar          #+#    #+#             */
/*   Updated: 2024/07/21 22:20:50 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.h"
#include "memory_map.h"
#include "options.h"
#include "version.h"

#include "libft/ft.h"

#include <elf.h>

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
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

static int ft_nm(struct config const *config, char const *files[], int n);

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
	}

	Elf64_Shdr const *section;

	section = &shdr[st_shndx];

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
		if (st_bind == STB_GLOBAL) {
			sym_char = 'R';
		} else {
		sym_char = 'D';
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

	while (1) {
		while (*s1 == '_') {
			s1 += 1;
		}

		while (*s2 == '_') {
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

static int ft_nm_elf64(struct config const *config, struct memory_map *mm, Elf64_Ehdr const *ehdr, char const *file)
{
	Elf64_Shdr const *shdr = (Elf64_Shdr const *)((unsigned char const *)mm->map + ehdr->e_shoff);

	Elf64_Shdr const *section_string_table_shdr = &shdr[ehdr->e_shstrndx];
	char const *section_header_string_table = (char const *)((unsigned char const *)mm->map + section_string_table_shdr->sh_offset);

	char const *symbol_string_table = NULL;
	char const *dynamic_string_table = NULL;

	for (Elf64_Half i = 0; i < ehdr->e_shnum; i += 1) {
		Elf64_Shdr const *section = &shdr[i];
		char const *section_name = &section_header_string_table[section->sh_name];

		if (section->sh_type != SHT_STRTAB) {
			continue;
		}

		if (ft_strcmp(section_name, ".strtab") == 0) {
			symbol_string_table = (char const *)((unsigned char const *)mm->map + section->sh_offset);
		} else if (ft_strcmp(section_name, ".dynstr") == 0) {
			dynamic_string_table = (char const *)((unsigned char const *)mm->map + section->sh_offset);
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
		case SHT_DYNSYM:
			string_table = dynamic_string_table;
			break;

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
			char const *symbol_name = &string_table[symbol->st_name];

			char type_char = _elf64_symbol_type_char(symbol, shdr);

			if (type_char == 'u' && symbol_name[0] == '\0') {
				continue;
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
			case 'r':
			case 'R':
			case 't':
			case 'T':
			case 'W':
				symbols[sym_i].has_address = 1;
				symbols[sym_i].offset = symbol->st_value;
				break;
			default:
				symbols[sym_i].has_address = 0; // TODO
				break;
			}

			sym_i += 1;
		}
	}

	if (!config->no_sort) {
		// TODO implement ft_qsort
		qsort(symbols, sym_i, sizeof(*symbols), _compare_symbol);
	}

	char offbuf[16 + 1];
	struct symbol *symbol;

	for (size_t i = 0; i < sym_i; i += 1) {
		symbol = &symbols[i];

		if (i != 0) {
			struct symbol *prev = &symbols[i - 1];
			char const *version_delim = ft_strchr(symbol->name, '@');

			if ((version_delim != NULL && ft_strncmp(prev->name, symbol->name, (size_t)(version_delim - symbol->name)) == 0) || strcmp(prev->name, symbol->name) == 0) {
				continue;
			}
		}

		if (symbol->has_address) {
			snprintf(offbuf, sizeof(offbuf), "%016lx", symbol->offset);
		} else {
			memset(offbuf, ' ', sizeof(offbuf) - 1);
		}

		write(STDOUT_FILENO, offbuf, sizeof(offbuf) - 1);
		write(STDOUT_FILENO, " ", 1);
		write(STDOUT_FILENO, &symbol->type_char, 1);
		write(STDOUT_FILENO, " ", 1);
		write(STDOUT_FILENO, symbol->name, ft_strlen(symbol->name));
		write(STDOUT_FILENO, "\n", 1);
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

static int ft_nm(struct config const *config, char const *files[], int n)
{
	int result;
	int tmp;

	result = EXIT_SUCCESS;
	for (int i = 0; i < n; i += 1) {
		tmp = ft_nm_file(config, files[i]);
		if (tmp > result) {
			result = tmp;
		}
	}
	return result;
}
