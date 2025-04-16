/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   algo1.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 17:10:39 by ankim             #+#    #+#             */
/*   Updated: 2025/04/16 15:46:09 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

t_node *find_min(t_node *stack)
{
    long    min; 
    t_node *min_node;
    t_node *tmp;

    if (!stack)
        return(NULL);
    tmp = stack->next;
    min = LONG_MAX;
    while (tmp != stack)
    {
        if (tmp->data < min)
        {
            min = tmp->data;
            min_node = tmp;
        }
        tmp = tmp->next;
    }
    return(min_node);
}

t_node *find_max (t_node *stack)
{
    long    max;
    t_node *max_node;
    t_node *tmp;

    if (!stack)
        return(NULL);
    max = LONG_MIN;
    tmp = stack->next;
    while (tmp != stack)
    {
        if (tmp->data > max)
        {
            max = tmp->data;
            max_node = tmp;
        }
        tmp = tmp->next;
    }
    return (max_node);
}

void node_a(t_node *stack_a, t_node *stack_b)
{
    current_pos(stack_a); // refreshes each position
    current_pos(stack_b);
    target_a(stack_a, stack_b);
    cost_a(stack_a, stack_b);
    set_cheapest(stack_a);
}

void current_pos(t_node *stack) // sets index of current node + sets abv median data
{
    int index;
    int mid;
    t_node *current;
    
    index = 0;
    if (!stack)
        return ;
    mid = stack_size(stack) / 2;
    // stack->id = index;
    // if (current->data <= median)
    //     current->above_mid = true;
    // else
    //     current->above_mid = false;
    // current = current->next;
    // index++;
	current = stack;
	while (current->next != stack)
	{
		current->id = index;
		if (index <= mid)
			current->above_mid = true;
		else
			current->above_mid = false;
		current = current->next;
		index++;
	}
}

void   target_a (t_node *stack_a, t_node *stack_b)
{
    t_node *current_b;
    t_node *target_node;
    t_node *current;
    long    best_match_index; // track of the closest smaller # thusfar

	if (!stack_a)
		return ;
    current = stack_a->next;
    while (current != stack_a)
    {
        best_match_index = LONG_MIN;
        current_b = stack_b->next;
        while (current_b != stack_b)
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
