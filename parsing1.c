/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 16:09:47 by ankim             #+#    #+#             */
/*   Updated: 2025/04/16 14:16:43 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

char **ft_process(char **str)
{
	char **s;

    if (ft_strchr(str[1], ' '))
    {
        s = ft_split(str[1], ' ');
        if (!s)
            return (NULL);
    }
	else
		{
			s = malloc(sizeof(char *) * 2); // x 2 to accomodate for "/"
			if (!s)
				return (NULL);
			s[0] = ft_strdup(str[1]);
			if (!s[0])
			{
				free(s);
				return (NULL);
			}
			s[1] = NULL;
		}
	return(s);
}

int	ft_check1(int argc, char **argv)
{
	int		i;
	int		y;

	i = 0;
	y = 1;
	if (argc < 2)
    {
        write (1, "ERROR", 6);
		return (-1);
    }
	while (y < argc)
	{
		i = 0;
		while (argv[y][i] != '\0')
		{
			if (!((argv[y][i] >= '0' && argv[y][i] <= '9') || argv[y][i] == '+'
					|| argv[y][i] == '-' || argv[y][i] == ' '))
				return (-1);
            i++;
		}
		y++;
	}
	return (0);
}

char**	ft_printme(int argc, char **argv)
{
	int check_results;
	
	check_results = ft_check1(argc, argv);
	if (argc == 2)
	{
		if (check_results == -1)
			write(1, "ERROR\n", 6);
		else
			return (ft_process(argv));
	}
	else
	{
		if (check_results == -1)
			write(1, "ERROR\n", 6);
		else if (check_results == 0)
			return(argv + 1);
	}
	return(NULL);
}

void printstack(t_node *headnode)
{
	t_node *current;
	
	current = headnode;
	while (1)
	{
		ft_printf("%d \n", current->data);
		current = current->next;
		if (current == headnode)
			break ;
	}
}

int	stack_size(t_node *stack)
{
	t_node *current;

	if (!stack)
		return (0);
	int size = 1;
	current = (stack)->next;
	while (current != stack)
	{
		size++;
		current = (current)->next;
	}
	return (size);
}

