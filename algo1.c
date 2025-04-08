/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   algo2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 17:10:39 by ankim             #+#    #+#             */
/*   Updated: 2025/04/08 17:50:34 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void node_a(t_node *stack_a, t_node *stack_b)
{
    current_pos(stack_a);
    current_pos(stack_b);
    target_a(stack_a, stack_b);
    cost_a(stack_a, stack_b);
    cheapest(stack_a);
}

void current_pos(t_node *stack)
{
    int current;
    int median;
    
    current = 0;
    if (!stack)
        return ;
    median = stack_size(stack) / 2;
    while (stack)
    {
        stack->id = current;
        if (current <= median)
            stack->above_median = true;
        else
            stack->above_median = false;
        stack = stack->next;
        current++;
    }
}

static void target_a (t_node *stack_a, t_node *stack_b)
{
    t_node *current_b;
    t_node *target_node;
    long    best_match_index; // track of the closest smaller # thusfar

    while (stack_a)
    {
        best_match_index = LONG_MIN;
        current_b = stack_b;
        while (current_b)
        {
            if ((current_b->data < stack_a->data)
                && (current_b->data > best_match_index))
            {
                best_match_index = current_b->data;
                target_node = current_b;
            }
            current_b = current_b->next;
        }
        if (best_match_index == LONG_MIN)
            stack_a->target_node = find_max(stack_b);
        else
            stack_a->target_node = target_node;
        stack_a = stack_a->next;
    }
}

static void cost_a(t_node *stack_a, t_node *stack_b)
{
    int size_a;
    int size_b;

    size_a = stack_size(stack_a);
    size_b = stack_size(stack_b);
    while (stack_a)
    {
        stack_a->push_cost = stack_a->id;
        if (!(stack_a->above_median))
            stack_a->push_cost = size_a - (stack_a->id);
        if (stack_a->target_node->above_median)
            stack_a->push_cost += stack_a->target_node->id;
        else
            stack_a->push_cost += size_b - (stack_a->target_node->id);
        stack_a = stack_a->next;
    }
}

void cheapest (t_node *stack)
{
    long    cheapest_value;
    t_node  *cheapest_node;

    if (!stack)
        return ;
    cheapest_value = LONG_MAX;
    while (stack)
    {
        if (stack->push_cost < cheapest_value)
        {
            cheapest_value = stack->push_cost;
            cheapest_node = stack;
        }
        stack = stack->next; 
    }
    cheapest_node->cheapest = true;
}