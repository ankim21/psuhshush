/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 12:32:14 by ankim             #+#    #+#             */
/*   Updated: 2025/03/20 17:37:58 by ankim            ###   ########.fr       */
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

typedef struct s_node {
	int data;
	int id;
	struct s_node *next;
	struct s_node *prev;
} t_node;

char	**ft_process(char **str);
int		ft_check1(int argc, char **argv);
char	**ft_printme(int argc, char **argv);
void	printstack(t_node *headnode);
int		check_duplicates(t_node *headnode);
int		add_node (int data, t_node **headnode, int i);
int		create_headnode(int data, t_node **headnode);
int		create_stack(int data, t_node **stack_a, int i);

//				swap.c
void    swap_a(t_node **stack_a);

#endif