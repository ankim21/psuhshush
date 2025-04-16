/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pushalgo.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 15:34:49 by ankim             #+#    #+#             */
/*   Updated: 2025/04/16 17:23:08 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void    small_sort (t_node **stack_a)
{
    int size;
    
    size = stack_size(*stack_a);
    if (size == 1)
    return ;
    if (size == 2)
    {
        if ((*stack_a)->data > (*stack_a)->next->data)
            swap_a(stack_a);
        else
            return ;
    }
    if (size == 3)
    {
        if ((*stack_a)->data > (*stack_a)->next->data && (*stack_a)
        ->data > (*stack_a)->next->next->data)
            rotate_a(stack_a);
        else if (((*stack_a)->next->data) > ((*stack_a)->data) &&
        ((*stack_a)->next->data > (*stack_a)->next->next->data))
            reverse_rotate_a(stack_a);
        if ((*stack_a)->data > (*stack_a)->next->data) 
            swap_a(stack_a);
    }
}

void    turk_algo(t_node **stack_a, t_node **stack_b)
{
    int size_a;
    t_node *current_b;
    
    size_a = stack_size(*stack_a);
    if (size_a > 3)
    {
        push_b(stack_a, stack_b);
        if (size_a > 3)
            push_b(stack_a, stack_b);
        if (size_a > 3)
        {
            while (size_a > 3)
            {
                node_a(*stack_a, *stack_b);
                a_to_b(stack_a, stack_b);
                size_a--;
            }
        }
    }
    small_sort(stack_a);
    current_b = (*stack_b)->next;
    while (current_b != *stack_b)
    {
        node_b(*stack_a, *stack_b);
        b_to_a(stack_a, stack_b);
        current_b = current_b->next;
    }
    current_pos(*stack_a);
    min_on_top((*stack_a));
}
