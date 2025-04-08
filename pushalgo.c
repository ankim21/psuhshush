/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pushalgo.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 15:34:49 by ankim             #+#    #+#             */
/*   Updated: 2025/04/08 17:53:52 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int cost_a (t_node *stack_a, int value)
{
    int a_size = stack_size(stack_a);
    t_node *current = stack_a;
    int pos_a = 0;

    while (current != stack_a)
    {
        if (current->data == value)
            break ;
        pos_a++;
        current = current->next;
    }
    if (pos_a > a_size / 2)
        return(a_size - pos_a);
    else
        return (pos_a);
}

int cost_b (t_node *stack_b, int value_to_push)
{
    t_node *current;
    int     pos;
    int     new_size;

    pos = 0;
    new_size = stack_size(stack_b);
    if (!stack_b || stack_b->data < value_to_push)
        return (0);
    current = stack_b->next;
    while (current != stack_b)
    {
        if (value_to_push > current->data)
            break ;
        pos++;
        current = current->next;
    }
    if (pos > new_size / 2)
        return (new_size - pos); //rrb
    return(pos); // rb
}

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

void    sort_stacks(t_node **stack_a, t_node **stack_b)
{
    int size_a;

    size_a = stack_size(stack_a);
    if (size_a > 3)
        push_b(stack_a, stack_b);
    if (size_a > 3) 
        push_b(stack_a, stack_b);
    while (size_a > 3)
    {
        node_a(*stack_a, *stack_b);
        a_to_b(sta   a, b);
    }

}

// void    turk_algo (t_node **stack_a, t_node **stack_b)
// {
//     int size_a;
//     t_node *start;
//     t_node *current;
//     int min_val;
//     int min_id;
//     int cost_a_val;
//     int cost_b_val;

//     size_a = stack_size(*stack_a);
//     if (size_a <= 3)
//     {
//         small_sort(stack_a);
//         return;
//     }
//     while (size_a > 3)
//     {
//         start = *stack_a;
//         current = start;
//         min_val = current->data;
//         min_id = current->id;
//         while (current->next != *stack_a)
//         {
//             current = current->next;
//             if (current->data < min_val)
//             {
//                 min_val = current->data;
//                 min_id = current->id;
//             }
//         }
//         cost_a_val = cost_a(*stack_a, min_id);
//         cost_b_val = cost_b(*stack_b, min_val);
//         if (cost_a_val > 0)
//         {
//             if (cost_a_val <= size_a / 2)
//             {
//                 while (cost_a_val > 0)
//                 {
//                     rotate_a(stack_a);
//                     cost_a_val--;    
//                 }
//             }
//             else
//             {
//                 while (cost_a_val > 0)
//                 {
//                     reverse_rotate_a(stack_a);
//                     cost_a_val--;    
//                 }
//             }
//         }
//         if (cost_b_val > 0)
//         {
//             if (cost_b_val <= (stack_size(*stack_b) + 1) / 2)
//                 while (cost_b_val > 0)
//                 {
//                     rotate_b(stack_b);
//                     cost_b_val--;
//                 }
//             else
//                 while (cost_b_val > 0)
//                 {
//                     reverse_rotate_b(stack_b);
//                     cost_b_val--;
//                 }
//         }
//         push_b(stack_a, stack_b);
//         size_a = stack_size(*stack_a);
//     }
//     small_sort(stack_a);
//     while (*stack_b)
//     {
//         push_a(stack_a, stack_b);
//     }
//     return ;
// }

