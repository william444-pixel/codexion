#include "../include/codexion.h"

long long	get_time_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (-1);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	custom_sleep(long long time_to_sleep, t_sim *sim)
{
	long long	start;

	start = get_time_ms();
	while (get_time_ms() - start < time_to_sleep)
	{
		pthread_mutex_lock(&sim->sim_lock);
		if (sim->stop_flag == 1)
		{
			pthread_mutex_unlock(&sim->sim_lock);
			break ;
		}
		pthread_mutex_unlock(&sim->sim_lock);
		usleep(500);
	}
}

void	print_status(t_coder *coder, char *status)
{
	long long	current_time;

	pthread_mutex_lock(&coder->sim->print_lock);
	pthread_mutex_lock(&coder->sim->sim_lock);
	if (coder->sim->stop_flag == 0)
	{
		current_time = get_time_ms() - coder->sim->start_time;
		printf("%lld %d %s\n", current_time, coder->id, status);
	}
	pthread_mutex_unlock(&coder->sim->sim_lock);
	pthread_mutex_unlock(&coder->sim->print_lock);
}

int	is_numeric(char *str)
{
	int	i;

	i = 0;
	if (!str || !str[0])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

void	clean_sim(t_sim *sim, t_coder *coders)
{
	int	i;

	i = -1;
	while (++i < sim->nb_coders)
		pthread_mutex_destroy(&sim->dongles[i].lock);
	pthread_mutex_destroy(&sim->print_lock);
	pthread_mutex_destroy(&sim->sim_lock);
	free(sim->dongles);
	free(coders);
}