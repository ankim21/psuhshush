/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   algo3.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 12:29:16 by ankim             #+#    #+#             */
/*   Updated: 2025/04/16 17:25:27 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void   target_b (t_node *stack_a, t_node *stack_b)
{
    t_node *current_a;
    t_node *target_node;
    t_node *current_b;
    long    best_match_index; // track of the closest smaller # thusfar

    current_b = stack_b->next;    
    while (current_b != stack_b)
    {
        printf("here\n");
        best_match_index = LONG_MAX;
        current_a = stack_a->next;
        while (current_a != stack_a)
        {
            if ((current_a->data > stack_b->data)
                && (current_a->data < best_match_index))
            {
                printf("here1\n");
                best_match_index = current_a->data;
                target_node = current_a;
            }
            current_a = current_a->next;
        }
        if (best_match_index == LONG_MAX)
            stack_b->target_node = find_min(stack_a);
        else
            stack_b->target_node = target_node;
        current_b = current_b->next;
    }
}

void b_to_a(t_node **stack_a, t_node **stack_b)
{
    set_top_node(stack_a, (*stack_b)->target_node, 'a');
    push_a(stack_a, stack_b);
}

void min_on_top(t_node *stack_a)
{
    t_node *tmp;

    tmp = find_min(stack_a);
    if (!tmp)
        return ;
    while (tmp != find_min(stack_a))
    {
        if (find_min(stack_a)->above_mid)
            rotate_a(&stack_a);
        else
            reverse_rotate_a(&stack_a);
    }
}