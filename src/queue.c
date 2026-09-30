/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nael-oua <nael-oua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 16:54:03 by nael-oua          #+#    #+#             */
/*   Updated: 2026/09/30 17:00:48 by nael-oua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

void	ft_swap(t_pq_node *a, t_pq_node *b)
{
	t_pq_node	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

void	pq_push(t_pqueue *pq, t_coder *coder, long long key)
{
	int	i;

	i = pq->size;
	pq->nodes[i].coder = coder;
	pq->nodes[i].key = key;
	pq->size++;
	while (i > 0 && pq->nodes[i].key < pq->nodes[(i - 1) / 2].key)
	{
		ft_swap(&pq->nodes[i], &pq->nodes[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}

void	pq_pop(t_pqueue *pq)
{
	if (pq->size == 0)
		return ;
	if (pq->size == 1)
	{
		pq->size = 0;
		return ;
	}
	ft_swap(&pq->nodes[0], &pq->nodes[1]);
	pq->size = 1;
}
