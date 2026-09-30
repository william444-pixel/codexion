#include "../include/codexion.h"

int	check_stop_or_single(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->sim_lock);
	if (coder->sim->stop_flag == 1)
	{
		pthread_mutex_unlock(&coder->sim->sim_lock);
		return (1);
	}
	pthread_mutex_unlock(&coder->sim->sim_lock);
	if (coder->sim->nb_coders == 1)
	{
		pthread_mutex_lock(&coder->left_dongle->lock);
		print_status(coder, "has taken a dongle");
		custom_sleep(coder->sim->time_to_burnout + 10, coder->sim);
		pthread_mutex_unlock(&coder->left_dongle->lock);
		return (1);
	}
	return (0);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->id % 2 == 0)
		usleep(1000);
	while (1)
	{
		if (check_stop_or_single(coder))
			break ;
		if (take_dongles(coder))
			break ;
		print_status(coder, "is compiling");
		pthread_mutex_lock(&coder->sim->sim_lock);
		coder->last_compile_start = get_time_ms();
		coder->compiles_done++;
		pthread_mutex_unlock(&coder->sim->sim_lock);
		custom_sleep(coder->sim->time_to_compile, coder->sim);
		drop_dongles(coder);
		print_status(coder, "is debugging");
		custom_sleep(coder->sim->time_to_debug, coder->sim);
		print_status(coder, "is refactoring");
		custom_sleep(coder->sim->time_to_refactor, coder->sim);
	}
	return (NULL);
}