/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 17:23:42 by ankim             #+#    #+#             */
/*   Updated: 2025/04/16 16:18:45 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void    push_a(t_node **stack_a, t_node **stack_b)
{ 
    t_node *topB;
    t_node *lastB; 

    if (!*stack_b)
        return ;
    if (!*stack_a)
        create_headnode((*stack_b)->data, stack_a);
    else
        add_node((*stack_b)->data, stack_a, ((*stack_a)->prev)->id + 1);
    if ((*stack_b)->next == *stack_b)
    {
        free(*stack_b);
        *stack_b = NULL;
    }
    else
    {
        topB = (*stack_b)->next;
        lastB = (*stack_b)-> prev;
        lastB->next = topB;
        topB->prev = lastB;
        free(*stack_b);
        *stack_b = topB;
    }
	current_pos(*stack_a);
	current_pos(*stack_b);
    ft_printf("pa\n");
}

void push_b(t_node **stack_a, t_node **stack_b)
{
    t_node *topA;
    t_node *lastA;
    int	size;

	size = stack_size(*stack_a);
    if (!*stack_a)
        return ;
    if (!*stack_b)
            create_headnode((*stack_a)->data, stack_b);
    else 
       add_node((*stack_a)->data, stack_b, ((*stack_b)->prev)->id +1);
    if ((*stack_a)->next == *stack_a)
    {
        free(*stack_a);
        *stack_a = NULL;
    }
    else
    {
        topA = (*stack_a)->next;
        lastA = (*stack_a)->prev;
        topA->prev = lastA;
        lastA->next = topA;
        free(*stack_a);
        *stack_a = topA;
		size--;
    }
	current_pos(*stack_a);
	current_pos(*stack_b);
    ft_printf("pb\n");
}

