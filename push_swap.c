/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 16:09:47 by ankim             #+#    #+#             */
/*   Updated: 2025/03/21 15:05:30 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

char **ft_process(char **str)
{
	int y; 
	int j;
	char **s;

	y = 1;
	j = 0;
    while (ft_strchr(str[1], ' '))
    {
        s = ft_split(str[1], ' ');
        if (!s)
            return (NULL);
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

int check_duplicates(t_node *headnode)
{
    t_node *current;
    t_node *runner;

    current = headnode;
    if (!headnode || headnode->next == headnode)
        return (0);
    while (1)
    {
        runner = current->next;
        while (runner != headnode)
        {
            if (current->data == runner->data)
            {
                write (1, "ERROR", 6);
                return (-1);
            }
            runner = runner->next;
        }
        current = current->next;
        if (current == headnode)
            break;
    }
    return (0);
}

int add_node (int data, t_node **headnode, int id)
{
	t_node *new;
	t_node *previous;

	previous = (*headnode)->prev;
	new = malloc (sizeof(t_node));
	if (!new)
		return (-1);
	new->data = data;
	new->next = *headnode;
	new->prev = previous;
    new->id = id;
	previous->next = new;
	(*headnode)->prev = new;
	return (0);
}

int	create_headnode(int data, t_node **headnode)
{
	*headnode = malloc(sizeof(t_node));
	if (!*headnode)
		return (-1);
	(*headnode)->data = data;
	(*headnode)->prev = *headnode;
	(*headnode)->next = *headnode;
	(*headnode)->id = 0;
	return (0);
}

int create_stack(int data, t_node **stack_a, int i)
{
	if (!*stack_a)
		return (create_headnode(data, stack_a));
	else 
		return (add_node(data, stack_a, i));
}

void    free_stack(t_node **stack_a, int argc)
{
    t_node *temp;
    int i;

    i = 0;
    while (i < argc)
    {
        temp = (*stack_a)->next;
        free((*stack_a));
        (*stack_a) = temp;
        i++;
    }
}

int	main(int argc, char **argv)
{
    char **result;
	int i;
	t_node *stack_a;

	stack_a = NULL;
	i = 0;
	if (argc < 2)
    {
        write (1, "ERROR", 6);
		return (-1);
    }
	result = ft_printme(argc, argv);
	if (!result)
		return (-1);
    while (result[i] != NULL)
    {
        if (create_stack(ft_atoi(result[i]), &stack_a, i) == -1)
            return (-1);
        i++;
    }
    if (check_duplicates(stack_a) == -1)
        return (-1);
    printstack(stack_a);
    // swap_a(&stack_a);
    printstack(stack_a);
    free_stack(&stack_a, argc - 1);
	return(0);
}
