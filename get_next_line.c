/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guletill <guletill@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/19 08:56:57 by guletill          #+#    #+#             */
/*   Updated: 2026/11/20 07:50:16 by guletill         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*read_file(int fd, char *save)
{
	char	*buffer;
	char	*temp;
	ssize_t	read_bytes;

	buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buffer)
		return (free(save), NULL);
	read_bytes = 1;
	while (!ft_strchr(save, '\n') && read_bytes > 0)
	{
		read_bytes = read(fd, buffer, BUFFER_SIZE);
		if (read_bytes == -1)
			break ;
		buffer[read_bytes] = '\0';
		temp = ft_strjoin(save, buffer);
		free(save);
		save = temp;
		if (!save)
			break ;
	}
	free(buffer);
	if (read_bytes == -1)
		return (free(save), NULL);
	return (save);
}

static char	*get_line(char *save)
{
	size_t	i;

	i = 0;
	if (!save[0])
		return (NULL);
	while (save[i] && save[i] != '\n')
		i++;
	if (save[i] == '\n')
		i++;
	return (ft_substr(save, 0, i));
}

static int	save_rest(char **save)
{
	size_t	i;
	char	*rest;

	i = 0;
	while ((*save)[i] && (*save)[i] != '\n')
		i++;
	if ((*save)[i] == '\n')
		i++;
	rest = NULL;
	if ((*save)[i])
	{
		rest = ft_substr(*save, i, ft_strlen(*save) - i);
		if (!rest)
		{
			free(*save);
			*save = NULL;
			return (-1);
		}
	}
	free(*save);
	*save = rest;
	return (0);
}

char	*get_next_line(int fd)
{
	static char	*save;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
	{
		free(save);
		save = NULL;
		return (NULL);
	}
	save = read_file(fd, save);
	if (!save)
		return (NULL);
	line = get_line(save);
	if (!line || save_rest(&save) == -1)
	{
		free(line);
		free(save);
		save = NULL;
		return (NULL);
	}
	return (line);
}

// int	main(void)
// {
// 	char	*line;
// 	int		fd;

// 	fd = open("note.txt", O_RDONLY);
// 	if (fd < 0)
// 		return (1);
// 	line = get_next_line(fd);
// 	while (line)
// 	{
// 		printf("%s\n", line);
// 		free(line);
// 		line = get_next_line(fd);
// 	}
// 	close(fd);
// 	return (0);
// }
