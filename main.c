/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benjamin <benjamin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/30 10:39:06 by benjamin          #+#    #+#             */
/*   Updated: 2024/06/30 23:35:07 by benjamin         ###   ########.fr       */
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
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static int ft_nm(char const *files[], int n);

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
		write(STDOUT_FILENO,
			"\n"
			"\n"
			"Copyright (C) 2024 Benjamin Brassart\n"
			"\n"
			"This program is free and open source software.\n"
			"You may redistribute it under the terms of the MIT license.\n"
			"\n"
			"Report bugs and issues at https://github.com/bemjaminbrassart/ft_nm\n", 216);
		return EXIT_SUCCESS;
	}

	if (argc == 1) {
		argv[1] = "a.out";
		argc += 1;
	}

	return ft_nm(&argv[1], argc - 1);
}

static int ft_nm_elf32(struct memory_map *mm, Elf32_Ehdr const *ehdr, char const *file)
{
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
		sym_char = 'D';
	} else {
		sym_char = '?';
	}

	if (st_bind == STB_LOCAL) {
		sym_char = (char)ft_tolower(sym_char);
	}

	return sym_char;
}

static int ft_nm_elf64(struct memory_map *mm, Elf64_Ehdr const *ehdr, char const *file)
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

			if (symbol_name[0] == '\0') {
				continue;
			}

			// char symbol_type_char = '?';

			// if (ELF64_ST_BIND(symbol->st_info) == STB_GNU_UNIQUE) {
			// 	symbol_type_char = 'u';
			// } else if (ELF64_ST_BIND(symbol->st_info) == STB_WEAK) {
			// 	if (symbol->st_shndx == SHN_UNDEF) {

			// 	}
			// }

			// if (symbol->st_shndx == SHN_UNDEF) {
			// 	symbol_type_char = 'U';
			// } else if ((symbol->st_info & 0x0F) == STT_FILE) {
			// 	symbol_type_char = 'a';
			// } else {
			// 	Elf64_Shdr const *symbol_section = &shdr[symbol->st_shndx];
			// 	char const *symbol_section_name = &section_header_string_table[symbol_section->sh_name];

			// 	if (symbol_section_name ==  section_names._bss) {
			// 		symbol_type_char = 'B';
			// 	} else if (symbol_section_name == section_names._data || symbol_section_name == section_names._dynamic || symbol_section_name == section_names._got_plt || symbol_section_name == section_names._data_rel_ro) {
			// 		symbol_type_char = 'D';
			// 	} else if (symbol_section_name == section_names._rodata || symbol_section_name == section_names._eh_frame_hdr) {
			// 		symbol_type_char = 'R';
			// 	} else if (symbol_section_name == section_names._text || symbol_section_name == section_names._init || symbol_section_name == section_names._fini) {
			// 		symbol_type_char = 'T';
			// 	} else {
			// 		symbol_type_char = '!';
			// 	}
			// }

			// switch ((symbol->st_info >> 4) & 0xF) {
			// case STB_LOCAL:
			// 	symbol_type_char = (char)tolower(symbol_type_char);
			// 	break;

			// case STB_WEAK:
			// 	if (symbol->st_shndx == SHN_UNDEF) {
			// 		symbol_type_char = 'w';
			// 	} else {
			// 		symbol_type_char = 'W';
			// 	}
			// 	break;

			// case STB_GNU_UNIQUE:
			// 	symbol_type_char = 'u';
			// 	break;

			// default:
			// 	break;
			// }

			char symbol_type_char = _elf64_symbol_type_char(symbol, shdr);

			write(STDOUT_FILENO, &symbol_type_char, 1);
			write(STDOUT_FILENO, " ", 1);
			write(STDOUT_FILENO, symbol_name, ft_strlen(symbol_name));
			write(STDOUT_FILENO, "\n", 1);
		}
	}

	(void)mm;
	(void)ehdr;
	(void)file;
	return EXIT_SUCCESS;
}

static int ft_nm_elf(struct memory_map *mm, Elf32_Ehdr const *ehdr, char const *file)
{
	switch (ehdr->e_ident[EI_CLASS]) {
	case ELFCLASS32:

		return ft_nm_elf32(mm, ehdr, file);

	case ELFCLASS64: {
		Elf64_Ehdr const *ehdr64;

		ehdr64 = mm_read(mm, NULL, sizeof(*ehdr64));
		if (ehdr64 == NULL) {
			return EXIT_FAILURE;
		}

		return ft_nm_elf64(mm, ehdr64, file);
	}

	default:
		return EXIT_FAILURE;
	}
}

static int ft_nm_map(void const *map, size_t map_size, char const *file)
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
		return ft_nm_elf(&root_map, ehdr, file);
	} else {
		return EXIT_FAILURE;
	}
}

static int ft_nm_file(char const *file)
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

		result = ft_nm_map(map, map_size, file);
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

static int ft_nm(char const *files[], int n)
{
	int result;
	int tmp;

	result = EXIT_SUCCESS;
	for (int i = 0; i < n; i += 1) {
		tmp = ft_nm_file(files[i]);
		if (tmp > result) {
			result = tmp;
		}
	}
	return result;
}
