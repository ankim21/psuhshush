/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rotate.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 16:19:30 by ankim             #+#    #+#             */
/*   Updated: 2025/04/16 16:15:34 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void rotate_a (t_node **stack_a)
{
    if (!*stack_a || (*stack_a)->next == *stack_a)
        return;
    *stack_a = (*stack_a)->next;
    current_pos(*stack_a);
    ft_printf("ra\n");
}

void rotate_b (t_node **stack_b)
{
    if (!*stack_b || (*stack_b)->next == (*stack_b))
        return ;
    *stack_b = (*stack_b)->next;
    current_pos(*stack_b);
    ft_printf("rb\n");
}

void rr (t_node **stack_a, t_node **stack_b, t_node *cheapest)
{
    while (*stack_b != cheapest->target_node && *stack_a != cheapest)
    {
        rotate_a(stack_a);
        rotate_b(stack_b);
        ft_printf("rr\n");
    }
    current_pos(*stack_a);
    current_pos(*stack_b);
}