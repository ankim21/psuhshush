/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   algo2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 17:51:23 by ankim             #+#    #+#             */
/*   Updated: 2025/04/08 18:13:03 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

t_node *get_cheapest(t_node **stack_a)
{
    while (*stack_a)
    {
        if ((*stack_a)->cheapest = true)
            return (*stack_a);
        else
            *stack_a = (*stack_a)->next;
    }
}

static void a_to_b(t_node **stack_a, t_node **stack_b)
{
    t_node *cheapest_node;

    cheapest_node = get_cheapest(*stack_a);
    if (cheapest_node->above_median && cheapest_node->target_node->above_median)
        rr(stack_a, stack_b, cheapest_node); 
    else if (!(cheapest_node)->above_median
    && !(cheapest_node->target_node->above_median))
        reverse_rotate_both(stack_a, stack_b, cheapest_node);
}