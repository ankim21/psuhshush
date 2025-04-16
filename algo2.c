/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   algo2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 17:51:23 by ankim             #+#    #+#             */
/*   Updated: 2025/04/16 17:10:05 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	ft_max (int a, int b)
{
	if (a > b)
		return (a);
	else
		return (b);
}

int	ft_min (int a, int b)
{
	if (a < b)
		return (a);
	else
		return (b);
}

void	cost_a(t_node *a, t_node *b) //a to be on top and target node to be on top
{
	int	size_a;
	int size_b;
    t_node *start;

    size_a = stack_size(a);
    size_b = stack_size(b);
    
    if (!a)
        return ;
    start = a->next;
    while (a != start)
    {
        if (a->above_mid)
        {
            if (a->target_node->above_mid)
                a->push_cost = ft_max(a->id, a->target_node->id);
            if (!a->target_node->above_mid)
                a->push_cost = a->id + (size_b - a->target_node->id);
        }
        if (!a->above_mid)
        {
            if (!a->target_node->above_mid)
                a->push_cost = ft_max((size_a - a->id), (size_b - a->target_node->id));
            if (a->target_node->above_mid)
                a->push_cost = (size_b - a->target_node->id) + a->target_node->id;
        }
        a = a->next;
    }
}

// void set_cheapest(t_node *stack) //uses push cost data for each node and it searches for the cheapest value
// {
//     long	cheapest_value; // lowest push cost, bool cheapest = true
// 	t_node	*current;
// 	t_node	*cheapest_node;
	
//     if (!stack)
//         return ;
//     current = stack->next;
//     cheapest_value = LONG_MAX;
//     while (current != stack)
//     {
// 		if (current->push_cost < cheapest_value)
// 		{
//             cheapest_value = current->push_cost;
// 			cheapest_node = current;
// 		}
// 		current = current->next;
//     }
// 	if (stack == cheapest_node)
// 		stack->cheapest = true;
// 	else
// 		stack->cheapest = false;
// }

void set_cheapest(t_node *stack) //uses push cost data for each node and it searches for the cheapest value
{
    long	cheapest_value; // lowest push cost, bool cheapest = true
	t_node	*current;
	
    if (!stack)
        return ;
    current = stack->next;
    cheapest_value = LONG_MAX;
    while (current != stack)
    {
		if (current->push_cost < cheapest_value)
            cheapest_value = current->push_cost;
		current = current->next;
    }
	current = stack->next;
	while (current != stack)
	{
		if (stack->push_cost == cheapest_value) 
			stack->cheapest = true;
		else
			current = current->next;
	}
}

void    set_top_node(t_node **stack, t_node *top, char name)
{
    if (!top)
        return ;
    while (*stack != top)
    {
        if (name == 'a')
        {
            if (top->above_mid)
                rotate_a(stack);
            else
                reverse_rotate_a(stack);
        }
        else if (name == 'b')
        {
            if (top->above_mid)
                rotate_b(stack);
            else
                reverse_rotate_b(stack);
        }
    }
}

t_node *get_cheapest(t_node *stack_a)
{
    t_node *current;

    if (!stack_a)
        return (NULL);
    current = stack_a->next;
    while (current != stack_a)
    {
        if (current->cheapest)
            return (current);
        else
            current = current->next;
    }
    return(current);
}

void a_to_b(t_node **stack_a, t_node **stack_b)
{
    t_node *cheapest_node;
	int	turns;

    if (!(*stack_a))
		return ;
	cheapest_node = get_cheapest(*stack_a);
    if (cheapest_node->above_mid && cheapest_node->target_node->above_mid)
	{
		turns = ft_min(cheapest_node->id, cheapest_node->target_node->id);
		while (turns >= 0)
		{
			rr(stack_a, stack_b, cheapest_node);
			turns--;
		} 
	}
    else if (cheapest_node && !(cheapest_node)->above_mid
    && cheapest_node->target_node && !(cheapest_node->target_node->above_mid))
	{
		turns = ft_min((stack_size(*stack_a) - cheapest_node->id), ((stack_size(*stack_b)) - cheapest_node->target_node->id));
		while (turns >= 0)
		{
			reverse_rotate_both(stack_a, stack_b, cheapest_node);
			turns--;
		} 
	}
    set_top_node(stack_a, cheapest_node, 'a');
    //if (cheapest_node)
    set_top_node(stack_b, cheapest_node->target_node, 'b');
    push_b(stack_a, stack_b);
}

void node_b(t_node *stack_a, t_node *stack_b)
{
    current_pos(stack_a); // refreshes each position
    current_pos(stack_b);
    target_b(stack_a, stack_b);
}
