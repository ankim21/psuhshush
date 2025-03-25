/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   algo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 15:34:49 by ankim             #+#    #+#             */
/*   Updated: 2025/03/25 18:20:19 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

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