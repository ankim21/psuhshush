/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 14:44:40 by ankim             #+#    #+#             */
/*   Updated: 2025/04/08 14:04:17 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	main(int argc, char **argv)
{
    char **result;
	int i;
	t_node *stack_a = NULL;
    t_node *stack_b = NULL;

	i = 0;
	if (argc < 2)
    {
        write (1, "ERROR", 6);
		return (-1);
    }
	result = ft_printme(argc, argv);
	if (!result)
		return (-1);
    while (result[i] != NULL)
    {
        if (create_stack(ft_atoi(result[i]), &stack_a, i) == -1)
            return (-1);
        i++;
    }
    if ((check_duplicates(stack_a) == -1))
        return (-1);
    if (is_sorted(&stack_a))
    {
        ft_printf("ERROR");
        return (-1);
    }
    turk_algo(&stack_a, &stack_b);
	return(0);
}
