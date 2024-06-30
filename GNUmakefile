# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    GNUmakefile                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: benjamin <benjamin@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/30 10:36:54 by benjamin          #+#    #+#              #
#    Updated: 2024/06/30 16:26:50 by benjamin         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

MAKEFILE := $(lastword $(MAKEFILE_LIST))
NAME := ft_nm

DIR_LIBFT := ./libft
NAME_LIBFT := libft.a
DIR_OBJ := .

SRC := main.c options.c config.c version.c memory_map.c
OBJ := $(SRC:%.c=$(DIR_OBJ)/%.c.o)
DEP := $(OBJ:.o=.d)

CFLAGS := -Wall -Wextra -Wconversion -g0 -O0
CPPFLAGS := -I . -I $(DIR_LIBFT)/include -std=c99 -MMD -MP

$(NAME): .EXTRA_PREREQS = $(DIR_LIBFT)/$(NAME_LIBFT)
$(NAME): $(OBJ)
	$(CC) $^ -o $@ -l:$(NAME_LIBFT) -L$(DIR_LIBFT)

$(OBJ): .EXTRA_PREREQS = $(MAKEFILE)
$(OBJ): $(DIR_OBJ)/%.c.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

version.c: version.c.in
	VERSION_MAJOR=1 VERSION_MINOR=0 VERSION_PATCH=0 VERSION_COMMIT=$(shell git rev-parse --short HEAD) envsubst < $< > $@

$(DIR_LIBFT)/$(NAME_LIBFT):
	$(MAKE) -C $(DIR_LIBFT) $(NAME_LIBFT)

-include $(DEP)

.PHONY: all clean fclean re

all: $(NAME)

clean:
	$(RM) $(OBJ) $(DEP)

fclean: clean
	$(RM) $(NAME) version.c

re: fclean
	$(MAKE) -f $(MAKEFILE) all
