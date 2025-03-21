/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   swap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 17:24:03 by ankim             #+#    #+#             */
/*   Updated: 2025/03/21 16:33:02 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void    swap_a(t_node **stack_a)
{
    t_node *temp;
    int data;

    data = (*stack_a)->data;
    temp = (*stack_a)->next;
    (*stack_a)->data = temp->data;
    temp->data = data;
}

void    swap_b(t_node **stack_b)
{
    t_node *temp;
    int data;

    data = (*stack_b)->data;
    temp = (*stack_b)->next;
    (*stack_b)->data = temp->data;
    temp->data = data;
}

void    swap_a_b(t_node **stack_a, t_node **stack_b)
{
    swap_a(stack_a);
    swap_b(stack_b);
}
