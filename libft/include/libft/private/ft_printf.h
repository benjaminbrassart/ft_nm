/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/09 10:01:25 by bbrassar          #+#    #+#             */
/*   Updated: 2023/01/17 07:26:02 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include <stddef.h>

typedef struct s_printerface	t_printerface;
typedef int						(*t_conversion)(t_printerface *, va_list);

// TODO add HEAP_BUFFER for asprintf
enum e_printf_type
{
	PF_FILE_DESCRIPTOR,
	PF_STACK_BUFFER,
};

struct s_printf_buffer
{
	char	*buffer;
	size_t	max_size;
	size_t	current_size;
};

union u_printerface
{
	int						fd;
	struct s_printf_buffer	buffer;
};

struct s_printerface
{
	enum e_printf_type	type;
	union u_printerface	iface;
};

int
ft_printf_core(t_printerface *pi, char const *fmt, va_list ap);

int
__print_string(t_printerface *pi, va_list ap);

int
__print_int(t_printerface *pi, va_list ap);

int
__print_char(t_printerface *pi, va_list ap);

int
__print_uint(t_printerface *pi, va_list ap);

int
__print_hex_low(t_printerface *pi, va_list ap);

int
__print_hex_up(t_printerface *pi, va_list ap);

size_t
__uitoa_base_s(
	unsigned long long n, char *buffer,
	size_t buffer_size, char const *base)
__attribute__((nonnull(2, 4)));

int
__printerface_write(t_printerface *interface, char const *data, size_t len);

#endif // FT_PRINTF_H
