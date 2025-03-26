/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   algo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 15:34:49 by ankim             #+#    #+#             */
/*   Updated: 2025/03/26 16:57:15 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int cost_a (t_node *stack_a, int value)
{
    int a_size = stack_size(stack_a);
    t_node *current = stack_a;
    int pos_a = 0;
    int checked = 0;

    while (checked == 0 || current != stack_a)
    {
        if (current->data == value)
            break ;
        pos_a++;
        current = current->next;
        checked++;
    }
    if (pos_a > a_size / 2)
        return(a_size - pos_a);
    else
        return (pos_a);
}

int cost_b (t_node *stack_b, int value)
{
    int b_size = stack_size (stack_b);
    t_node *current = stack_b;
    int pos_b = 0;
    int checked = 0;

    if (!stack_b)
        return (0);
    while (checked < b_size)
    {
        if (value < current->data)
            pos_b = checked + 1;
        else
        {
            pos_b = checked;
            break ;
        }
        current = current->next;
        checked++;
    }
    if (checked == b_size)
        pos_b = 0;
    if (pos_b > b_size / 2)
        return (b_size - pos_b);
    else
        return (pos_b);
}

void    turk_algo (t_node **stack_a, t_node **stack_b)
{
    int max;
    int min;
    
    push_b(stack_a, stack_b);
    push_b(stack_a, stack_b);
    if ((*stack_b)->data > (*stack_b)->next->data)
    {
        max = (*stack_b)->data;
        min = (*stack_b)->next->data; 
    }
    else
    {
        max = (*stack_b)->next->data;
        min = (*stack_b)->data;
    }
    while (*stack_a)
    {
        max
    }
}