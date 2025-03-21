/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 17:23:42 by ankim             #+#    #+#             */
/*   Updated: 2025/03/21 17:47:46 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"
void    push_a(t_node **stack_a, t_node **stack_b)
{ 
    t_node *topA;
    t_node *topB;

    if (!topB)
        return ;
    if (!topA)
        return(create_headnode(topB->data, &stack_a));
    topA = topB->data;
    topB->next = topA;
    
}