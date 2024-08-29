/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conversions.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/17 05:57:43 by bbrassar          #+#    #+#             */
/*   Updated: 2023/05/25 07:58:27 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONVERSIONS_H
# define CONVERSIONS_H

# include "libft/private/ft_printf.h"

static t_conversion const	g_conversions[256] = {
['s'] = __print_string,
['d'] = __print_int,
['i'] = __print_int,
['c'] = __print_char,
['u'] = __print_uint,
['x'] = __print_hex_low,
['X'] = __print_hex_up,
};

#endif // CONVERSIONS_H
