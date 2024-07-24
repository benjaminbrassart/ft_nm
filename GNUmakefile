# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    GNUmakefile                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/30 10:36:54 by bbrassar          #+#    #+#              #
#    Updated: 2024/07/24 08:14:15 by bbrassar         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

override MAKEFILE := $(lastword $(MAKEFILE_LIST))
NAME := ft_nm

DIR_LIBFT := ./libft
NAME_LIBFT := libft.a
DIR_OBJ := .

override SRC := main.c options.c config.c version.c memory_map.c ft_qsort.c bswap.c
override OBJ := $(SRC:%.c=$(DIR_OBJ)/%.c.o)
override DEP := $(OBJ:.o=.d)

override _CFLAGS := -Wall -Wextra -Wconversion
override _CPPFLAGS := -I . -I $(DIR_LIBFT)/include -std=c11 -MMD -MP -D_GNU_SOURCE
override _LDFLAGS := -L$(DIR_LIBFT)
override _LDLIBS := -l:$(NAME_LIBFT)

$(NAME): .EXTRA_PREREQS = $(DIR_LIBFT)/$(NAME_LIBFT)
$(NAME): $(OBJ)
	$(CC) $^ -o $@ $(_LDFLAGS) $(LDFLAGS) $(_LDLIBS) $(LDLIBS)

$(DIR_OBJ)/bswap.c.o: CFLAGS += -O2
$(OBJ): .EXTRA_PREREQS = $(MAKEFILE)
$(OBJ): $(DIR_OBJ)/%.c.o: %.c
	@mkdir -p $(@D)
	$(CC) $(_CFLAGS) $(CFLAGS) $(_CPPFLAGS) $(CPPFLAGS) -c $< -o $@

version.c: version.c.in
	@VERSION_MAJOR=1 VERSION_MINOR=0 VERSION_PATCH=0 \
	VERSION_COMMIT=$(shell git rev-parse --short HEAD)$(shell git diff-index --quiet HEAD || echo -dirty) \
	envsubst < $< > $@

$(DIR_LIBFT)/$(NAME_LIBFT):
	$(MAKE) -C $(DIR_LIBFT) $(NAME_LIBFT)

-include $(DEP)

.PHONY: all clean fclean re

all: $(NAME) $(ASM)

clean:
	$(RM) $(OBJ) $(DEP) $(ASM)

fclean: clean
	$(RM) $(NAME) version.c

re: fclean
	$(MAKE) -f $(MAKEFILE) all
