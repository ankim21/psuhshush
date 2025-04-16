/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 12:32:14 by ankim             #+#    #+#             */
/*   Updated: 2025/04/16 17:08:08 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

#include "LIBFT/libft.h"
#include "PRINTF/ft_printf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>

typedef struct s_node {
	int 	data;
	int 	id;
	int		push_cost;
	bool 	above_mid;
	bool 	cheapest;
	struct s_node *target_node;
	struct s_node *next;
	struct s_node *prev;
} t_node;

char	**ft_process(char **str);
int		ft_check1(int argc, char **argv);
char	**ft_printme(int argc, char **argv);
void	printstack(t_node *headnode);
int		stack_size(t_node *stack);
int		check_duplicates(t_node *headnode);
int		add_node (int data, t_node **headnode, int i);
int		create_headnode(int data, t_node **headnode);
int		create_stack(int data, t_node **stack_a, int i);
void    free_stack(t_node **stack_a);
bool	is_sorted (t_node **stack_a);
void    push_a(t_node **stack_a, t_node **stack_b);
void    push_b(t_node **stack_a, t_node **stack_b);
void 	rotate_a (t_node **stack_a);
void 	rotate_b (t_node **stack_b);
void 	rr (t_node **stack_a, t_node **stack_b, t_node *cheapest);
void 	reverse_rotate_a(t_node **stack_a);
void 	reverse_rotate_b(t_node **stack_b);
void 	reverse_rotate_both (t_node **stack_a, t_node **stack_b, t_node *cheapest);
void    swap_a(t_node **stack_a);
void    swap_b(t_node **stack_b);
void    swap_a_b(t_node **stack_a, t_node **stack_b);
t_node 	*find_min(t_node *stack);
t_node 	*find_max(t_node *stack);
int 	ft_max (int a, int b);
int		ft_min (int a, int b);
t_node *get_cheapest(t_node *stack_a);
void 	node_a(t_node *stack_a, t_node *stack_b);
void 	current_pos(t_node *stack);
void	target_a (t_node *stack_a, t_node *stack_b);
void    cost_a(t_node *stack_a, t_node *stack_b);
void 	set_cheapest(t_node *stack);
void    set_top_node (t_node **stack, t_node *top, char name);
void 	a_to_b(t_node **stack_a, t_node **stack_b);
void 	node_b(t_node *stack_a, t_node *stack_b);
void   	target_b (t_node *stack_a, t_node *stack_b);
void 	b_to_a(t_node **stack_a, t_node **stack_b);
void 	min_on_top(t_node *stack_a);
void    small_sort (t_node **stack_a);
void    turk_algo(t_node **stack_a, t_node **stack_b);

#endif