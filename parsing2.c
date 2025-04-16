/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 14:47:17 by ankim             #+#    #+#             */
/*   Updated: 2025/04/16 11:33:33 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

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
    new->push_cost = 0;
    new->above_mid = false;
    new->cheapest = false;
    new->target_node= NULL;
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
    (*headnode)->push_cost = 0;
    (*headnode)->above_mid = false;
    (*headnode)->cheapest = false;
    (*headnode)->target_node= NULL;
	return (0);
}

int create_stack(int data, t_node **stack_a, int i)
{
	if (!*stack_a)
		return (create_headnode(data, stack_a));
	else 
		return (add_node(data, stack_a, i));
}

void    free_stack(t_node **stack_a)
{
    t_node *temp;
    int i;
    int size = stack_size(*stack_a);

    i = 0;
    while (i < size)
    {
        temp = (*stack_a)->next;
        free((*stack_a));
        (*stack_a) = temp;
        i++;
    }
}
	