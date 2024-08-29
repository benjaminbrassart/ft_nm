/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 15:26:24 by bbrassar          #+#    #+#             */
/*   Updated: 2023/11/28 12:20:31 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft.h"
#include <stdlib.h>
#include <unistd.h>

static void	gnl_join(char **line, char *buffer, size_t n)
{
	size_t	i;
	char	*s;

	i = 0;
	if (*line != NULL)
		while ((*line)[i])
			++i;
	s = malloc(i + n + 1);
	if (s != NULL)
	{
		ft_memcpy(s, *line, i);
		ft_memcpy(s + i, buffer, n);
		s[i + n] = 0;
		free(*line);
		*line = s;
	}
}

static int	gnl_copy(char **line, char *buffer, char **rest)
{
	int	i;

	if (line == NULL)
		return (0);
	i = 0;
	while (buffer[i] != '\0' && buffer[i] != '\n')
		++i;
	gnl_join(line, buffer, i);
	free(*rest);
	if (buffer[i] != '\0')
		*rest = ft_strdup(buffer + i + 1);
	else
		*rest = NULL;
	i = (buffer[i] != '\0');
	free(buffer);
	return (i);
}

int	get_next_line(int fd, char **line)
{
	static char	*rest = NULL;
	char		*buffer;
	int			bytes;

	if (line == NULL || BUFFER_SIZE < 1)
		return (-1);
	bytes = 1;
	*line = NULL;
	if (rest != NULL)
		if (gnl_copy(line, ft_strdup(rest), &rest))
			return (1);
	while (bytes > 0)
	{
		buffer = malloc(sizeof (char) * (BUFFER_SIZE + 1));
		bytes = read(fd, ft_memset(buffer, 0, BUFFER_SIZE + 1), BUFFER_SIZE);
		if (bytes == -1)
			free(buffer);
		else if (gnl_copy(line, buffer, &rest))
			return (1);
	}
	return (bytes);
}
