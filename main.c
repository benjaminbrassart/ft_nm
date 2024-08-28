/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 10:39:06 by bbrassar          #+#    #+#             */
/*   Updated: 2024/08/28 15:50:24 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bswap.h"
#include "config.h"
#include "elf_utils.h"
#include "memory_map.h"
#include "options.h"
#include "version.h"

#include "ft_ext.h"
#include "libft/ft.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static char const COPYRIGHT[] =
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
		write(STDOUT_FILENO, COPYRIGHT, sizeof(COPYRIGHT) - 1);
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

static void _fill_valbuf(char buf[], size_t n, uint64_t value)
{
	static char const base[16] = "0123456789abcdef";

	ft_memset(buf, '0', n);
	while (n > 0 && value > 0) {
		buf[n - 1] = base[value % sizeof(base)];
		value /= sizeof(base);
		n -= 1;
	}
}

static int _ft_nm_elf(struct config const *config, struct memory_map const *mm,
		      Elf_Ehdr const *ehdr, char const *file)
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

	Elf64_Off const e_shoff = ELF_GET(elfclass, elfdata, ehdr, e_shoff);

	uint8_t const *raw_map = mm->start;
	uint8_t const *raw_shdr = &raw_map[e_shoff];

	if (!mm_check(mm, raw_shdr, ELF_SIZE(elfclass, Elf_Shdr))) {
		return EXIT_FAILURE;
	}

	Elf_Shdr const *symtab_shdr = NULL;
	Elf_Shdr const *strtab_shdr = NULL;
	char const *string_table = NULL;
	Elf64_Word string_table_length = 0;

	Elf64_Xword sym_count = 0;

	/*
	   TODO from documentation:

	   If the number of entries in the section header table is
	   larger than or equal to SHN_LORESERVE (0xff00), e_shnum
	   holds the value zero and the real number of entries in the
	   section header table is held in the sh_size member of the
	   initial entry in section header table.  Otherwise, the
	   sh_size member of the initial entry in the section header
	   table holds the value zero.
	 */

	Elf64_Half const e_shentsize =
		ELF_GET(elfclass, elfdata, ehdr, e_shentsize);
	Elf64_Half const e_shnum = ELF_GET(elfclass, elfdata, ehdr, e_shnum);

	if (!mm_check(mm, raw_shdr, e_shnum * e_shentsize)) {
		return EXIT_FAILURE;
	}

	for (Elf64_Half i = 0; i < e_shnum; i += 1) {
		Elf_Shdr const *section =
			(Elf_Shdr const *)&raw_shdr[i * e_shentsize];
		Elf64_Word const sh_type =
			ELF_GET(elfclass, elfdata, section, sh_type);

		if (sh_type != SHT_SYMTAB) {
			continue;
		}

		Elf64_Word const sh_link =
			ELF_GET(elfclass, elfdata, section, sh_link);

		if (sh_link >= e_shnum) {
			return EXIT_FAILURE;
		}

		if (sh_link == SHN_UNDEF) {
			return EXIT_FAILURE;
		}

		symtab_shdr = section;
		strtab_shdr =
			(Elf_Shdr const *)&raw_shdr[sh_link * e_shentsize];

		Elf64_Xword const sh_size =
			ELF_GET(elfclass, elfdata, symtab_shdr, sh_size);
		Elf64_Xword const sh_entsize =
			ELF_GET(elfclass, elfdata, symtab_shdr, sh_entsize);

		if (sh_size % sh_entsize != 0) {
			return EXIT_FAILURE;
		}

		sym_count = sh_size / sh_entsize;
		string_table = (char const *)&raw_map[ELF_GET(
			elfclass, elfdata, strtab_shdr, sh_offset)];
		string_table_length = (Elf64_Word)ELF_GET(elfclass, elfdata,
							  strtab_shdr, sh_size);

		if (!mm_check(mm, string_table, string_table_length)) {
			return EXIT_FAILURE;
		}

		break;
	}

	if (sym_count == 0) {
		write(STDERR_FILENO, "ft_nm: ", 7);
		write(STDERR_FILENO, file, ft_strlen(file));
		write(STDERR_FILENO, ": no symbols\n", 13);

		return EXIT_SUCCESS;
	}

	char const *string_table_end = &string_table[string_table_length];
	struct symbol *symbols = NULL;

	symbols = ft_calloc((size_t)sym_count, sizeof(*symbols));
	if (symbols == NULL) {
		return EXIT_FAILURE;
	}

	uint8_t const *raw_symbol_table =
		&raw_map[ELF_GET(elfclass, elfdata, symtab_shdr, sh_offset)];
	size_t sym_i = 0;

	for (Elf64_Xword j = 0; j < sym_count; j += 1) {
		Elf_Sym const *symbol = (Elf_Sym const *)&raw_symbol_table
			[j * ELF_SIZE(elfclass, Elf_Sym)];
		char const *symbol_name;
		size_t symbol_name_length;
		char type_char;

		uint8_t const st_info =
			ELF_GET(elfclass, elfdata, symbol, st_info);
		uint8_t const st_type = ELF_MATCH(elfclass,
						  ELF64_ST_TYPE(st_info),
						  ELF32_ST_TYPE(st_info));

		if (st_type == STT_SECTION) {
			if (!debug_symbols) {
				continue;
			}

			Elf64_Section const st_shndx =
				ELF_GET(elfclass, elfdata, symbol, st_shndx);
			Elf_Shdr const *section_symbol =
				(Elf_Shdr const *)&raw_shdr[st_shndx *
							    ELF_SIZE(elfclass,
								     Elf_Shdr)];

			if (st_shndx >= e_shnum) {
				return EXIT_FAILURE;
			}

			Elf64_Half const e_shstrndx =
				ELF_GET(elfclass, elfdata, ehdr, e_shstrndx);

			if (e_shstrndx >= e_shnum) {
				return EXIT_FAILURE;
			}

			Elf_Shdr const *section_strtab =
				(Elf_Shdr const *)&raw_shdr[e_shstrndx *
							    ELF_SIZE(elfclass,
								     Elf_Shdr)];

			Elf64_Off const sh_offset = ELF_GET(
				elfclass, elfdata, section_strtab, sh_offset);
			char const *section_string_table =
				(char const *)&raw_map[sh_offset];
			Elf64_Word const section_string_table_length =
				(Elf64_Word)ELF_GET(elfclass, elfdata,
						    section_strtab, sh_size);

			if (!mm_check(mm, string_table, string_table_length)) {
				return EXIT_FAILURE;
			}

			char const *section_string_table_end =
				&section_string_table
					[section_string_table_length];

			Elf64_Word const sh_name = ELF_GET(
				elfclass, elfdata, section_symbol, sh_name);

			if (sh_name >= section_string_table_length) {
				return EXIT_FAILURE;
			}

			if (sh_name == 0) {
				symbol_name = "";
				symbol_name_length = 0;
			} else {
				symbol_name = &section_string_table[sh_name];
				symbol_name_length = ft_strnlen(
					symbol_name,
					(size_t)(section_string_table_end -
						 symbol_name));
			}

			type_char = _elf_section_type_char(
				section_symbol, symbol_name, elfclass, elfdata);
		} else {
			Elf64_Word const st_name =
				ELF_GET(elfclass, elfdata, symbol, st_name);

			if (st_name >= string_table_length) {
				return EXIT_FAILURE;
			}

			if (st_name == 0) {
				symbol_name = "";
				symbol_name_length = 0;
			} else {
				symbol_name = &string_table[st_name];
				symbol_name_length = ft_strnlen(
					symbol_name, (size_t)(string_table_end -
							      symbol_name));
			}

			type_char = _elf_symbol_type_char(symbol, raw_shdr,
							  elfclass, elfdata);
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
			switch (type_char) {
			case 'B':
			case 'D':
			case 'R':
			case 'T':
			case 'U':
			case 'v':
			case 'V':
			case 'w':
			case 'W':
				break;

			default:
				continue;
			}
		}

		symbols[sym_i].name = symbol_name;
		symbols[sym_i].name_length = symbol_name_length;
		symbols[sym_i].type_char = type_char;

		switch (symbols[sym_i].type_char) {
		case 'a':
		case 'A':
		case 'b':
		case 'B':
		case 'c':
		case 'C':
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
			symbols[sym_i].value =
				ELF_GET(elfclass, elfdata, symbol, st_value);
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

	size_t sizeof_valbuf = ELF_MATCH(elfclass, 16, 8);
	char valbuf[16];
	char *valptr;

	valptr = &valbuf[16 - sizeof_valbuf];

	struct symbol *symbol;

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
			_fill_valbuf(valptr, sizeof_valbuf, symbol->value);
		} else {
			ft_memset(valptr, ' ', sizeof_valbuf);
		}

		write(STDOUT_FILENO, valptr, sizeof_valbuf);
		write(STDOUT_FILENO, " ", 1);
		write(STDOUT_FILENO, &symbol->type_char, 1);
		write(STDOUT_FILENO, " ", 1);
		write(STDOUT_FILENO, symbol->name, symbol->name_length);
		write(STDOUT_FILENO, "\n", 1);
	}

	free(symbols);
	return EXIT_SUCCESS;
}

static int _check_ehdr(char const *file, Elf_Ehdr const *ehdr)
{
	uint8_t const elfclass = ehdr->elf32.e_ident[EI_CLASS];

	if (elfclass != ELFCLASS32 && elfclass != ELFCLASS64) {
		write(STDERR_FILENO, "ft_nm: ", 7);
		write(STDERR_FILENO, file, ft_strlen(file));
		write(STDERR_FILENO, ": invalid ELF class\n", 20);
		return EXIT_FAILURE;
	}

	uint8_t const elfdata = ehdr->elf32.e_ident[EI_DATA];

	if (elfdata != ELFDATA2LSB && elfdata != ELFDATA2MSB) {
		write(STDERR_FILENO, "ft_nm: ", 7);
		write(STDERR_FILENO, file, ft_strlen(file));
		write(STDERR_FILENO, ": invalid ELF byte ordering\n", 28);
		return EXIT_FAILURE;
	}

	uint8_t const elfversion = ehdr->elf32.e_ident[EI_VERSION];

	if (elfversion != EV_CURRENT) {
		write(STDERR_FILENO, "ft_nm: ", 7);
		write(STDERR_FILENO, file, ft_strlen(file));
		write(STDERR_FILENO, ": invalid ELF version\n", 22);
		return EXIT_FAILURE;
	}

	Elf64_Half const e_type = ELF_GET(elfclass, elfdata, ehdr, e_type);

	if (e_type != ET_REL && e_type != ET_EXEC && e_type != ET_DYN) {
		write(STDERR_FILENO, "ft_nm: ", 7);
		write(STDERR_FILENO, file, ft_strlen(file));
		write(STDERR_FILENO, ": invalid ELF file type\n", 24);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

static int ft_nm_elf(struct config const *config, struct memory_map const *mm,
		     char const *file)
{
	Elf_Ehdr const *ehdr = mm->start;
	int check_header = _check_ehdr(file, ehdr);

	if (check_header != EXIT_SUCCESS) {
		return check_header;
	}

	return _ft_nm_elf(config, mm, ehdr, file);
}

static int ft_nm_map(struct config const *config, void const *map,
		     size_t map_size, char const *file)
{
	struct memory_map root_map = mm_new(map, map_size);
	uint8_t const *e_ident = map;

	do {
		if (!mm_check(&root_map, e_ident, EI_NIDENT)) {
			break;
		}

		if (!(ft_memcmp(e_ident, ELFMAG, SELFMAG) == 0)) {
			break;
		}

		if (config->print_file_name) {
			write(STDOUT_FILENO, "\n", 1);
			write(STDOUT_FILENO, file, ft_strlen(file));
			write(STDOUT_FILENO, ":\n", 2);
		}

		uint8_t const elfclass = e_ident[EI_CLASS];

		if (!mm_check(&root_map, map, ELF_SIZE(elfclass, Elf_Ehdr))) {
			break;
		}

		return ft_nm_elf(config, &root_map, file);
	} while (false);

	write(STDERR_FILENO, "ft_nm: ", 7);
	write(STDERR_FILENO, file, ft_strlen(file));
	write(STDERR_FILENO, ": file format not recognized\n", 29);
	return EXIT_FAILURE;
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
		fd = open(file, O_RDONLY | O_NONBLOCK);
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
