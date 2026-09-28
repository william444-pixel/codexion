#include "../include/codexion.h"

int	check_all_compiled(t_sim *sim, t_coder *coders)
{
	int	i;
	int	finished;

	if (sim->nb_compiles_req == -1)
		return (0);
	i = -1;
	finished = 0;
	while (++i < sim->nb_coders)
	{
		pthread_mutex_lock(&sim->sim_lock);
		if (coders[i].compiles_done >= sim->nb_compiles_req)
			finished++;
		pthread_mutex_unlock(&sim->sim_lock);
	}
	if (finished == sim->nb_coders)
	{
		pthread_mutex_lock(&sim->sim_lock);
		sim->stop_flag = 1;
		pthread_mutex_unlock(&sim->sim_lock);
		return (1);
	}
	return (0);
}

int	check_burnout(t_sim *sim, t_coder *coders)
{
	int			i;
	long long	time_since;

	i = -1;
	while (++i < sim->nb_coders)
	{
		pthread_mutex_lock(&sim->sim_lock);
		time_since = get_time_ms() - coders[i].last_compile_start;
		if (time_since >= sim->time_to_burnout)
		{
			sim->stop_flag = 1;
			pthread_mutex_unlock(&sim->sim_lock);
			pthread_mutex_lock(&sim->print_lock);
			printf("%lld %d burned out\n", get_time_ms() - sim->start_time, coders[i].id);
			pthread_mutex_unlock(&sim->print_lock);
			return (1);
		}
		pthread_mutex_unlock(&sim->sim_lock);
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_coder	*coders;
	t_sim	*sim;

	coders = (t_coder *)arg;
	sim = coders[0].sim;
	while (1)
	{
		if (check_burnout(sim, coders) == 1)
			break ;
		if (check_all_compiled(sim, coders) == 1)
			break ;
		usleep(1000);
	}
	return (NULL);
}