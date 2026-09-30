/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nael-oua <nael-oua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 16:54:09 by nael-oua          #+#    #+#             */
/*   Updated: 2026/09/30 17:07:40 by nael-oua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

long long	get_dongle_key(t_coder *coder)
{
	long long	deadline;

	if (coder->sim->scheduler == SIM_FIFO)
		return (get_time_ms());
	pthread_mutex_lock(&coder->sim->sim_lock);
	deadline = coder->last_compile_start + coder->sim->time_to_burnout;
	pthread_mutex_unlock(&coder->sim->sim_lock);
	return (deadline);
}

int	can_take_dongle(t_dongle *dongle, t_coder *coder)
{
	if (dongle->taken == 1)
		return (0);
	if (get_time_ms() < dongle->available_at)
		return (0);
	if (dongle->pq.size > 0 && dongle->pq.nodes[0].coder->id != coder->id)
		return (0);
	return (1);
}

int	request_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	while (1)
	{
		pthread_mutex_lock(&coder->sim->sim_lock);
		if (coder->sim->stop_flag)
		{
			pthread_mutex_unlock(&coder->sim->sim_lock);
			pthread_mutex_unlock(&dongle->lock);
			return (1);
		}
		pthread_mutex_unlock(&coder->sim->sim_lock);
		if (can_take_dongle(dongle, coder))
			break ;
		pthread_mutex_unlock(&dongle->lock);
		usleep(500);
		pthread_mutex_lock(&dongle->lock);
	}
	dongle->taken = 1;
	pq_pop(&dongle->pq);
	pthread_mutex_unlock(&dongle->lock);
	print_status(coder, "has taken a dongle");
	return (0);
}

int	take_dongles(t_coder *coder)
{
	if (coder->id == coder->sim->nb_coders)
	{
		pthread_mutex_lock(&coder->right_dongle->lock);
		pthread_mutex_lock(&coder->left_dongle->lock);
		pq_push(&coder->right_dongle->pq, coder, get_dongle_key(coder));
		pq_push(&coder->left_dongle->pq, coder, get_dongle_key(coder));
		pthread_mutex_unlock(&coder->right_dongle->lock);
		pthread_mutex_unlock(&coder->left_dongle->lock);
		if (request_dongle(coder, coder->right_dongle))
			return (1);
		if (request_dongle(coder, coder->left_dongle))
			return (1);
	}
	else
	{
		pthread_mutex_lock(&coder->left_dongle->lock);
		pthread_mutex_lock(&coder->right_dongle->lock);
		pq_push(&coder->left_dongle->pq, coder, get_dongle_key(coder));
		pq_push(&coder->right_dongle->pq, coder, get_dongle_key(coder));
		pthread_mutex_unlock(&coder->left_dongle->lock);
		pthread_mutex_unlock(&coder->right_dongle->lock);
		if (request_dongle(coder, coder->left_dongle))
			return (1);
		if (request_dongle(coder, coder->right_dongle))
			return (1);
	}
	return (0);
}

void	drop_dongles(t_coder *coder)
{
	long long	next_available;

	next_available = get_time_ms() + coder->sim->dongle_cooldown;
	pthread_mutex_lock(&coder->left_dongle->lock);
	coder->left_dongle->taken = 0;
	coder->left_dongle->available_at = next_available;
	pthread_mutex_unlock(&coder->left_dongle->lock);
	pthread_mutex_lock(&coder->right_dongle->lock);
	coder->right_dongle->taken = 0;
	coder->right_dongle->available_at = next_available;
	pthread_mutex_unlock(&coder->right_dongle->lock);
}
