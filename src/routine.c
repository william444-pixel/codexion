#include "../include/codexion.h"

void	wait_for_cooldown(t_dongle *dongle, t_sim *sim)
{
	long long	now;

	now = get_time_ms();
	if (now < dongle->available_at)
	{
		custom_sleep(dongle->available_at - now, sim);
	}
}

void	apply_edf(t_coder *coder)
{
	long long	my_dl;
	long long	l_dl;
	long long	r_dl;
	long long	t_burn;

	if (coder->sim->scheduler != SIM_EDF)
		return ;
	pthread_mutex_lock(&coder->sim->sim_lock);
	t_burn = coder->sim->time_to_burnout;
	my_dl = coder->last_compile_start + t_burn;
	l_dl = coder->left_neighbor->last_compile_start + t_burn;
	r_dl = coder->right_neighbor->last_compile_start + t_burn;
	pthread_mutex_unlock(&coder->sim->sim_lock);
	if (l_dl < my_dl || r_dl < my_dl)
		usleep(500);
}

void	take_dongles(t_coder *coder)
{
	apply_edf(coder);
	if (coder->id == coder->sim->nb_coders)
	{
		pthread_mutex_lock(&coder->right_dongle->lock);
		wait_for_cooldown(coder->right_dongle, coder->sim);
		print_status(coder, "has taken a dongle");
		pthread_mutex_lock(&coder->left_dongle->lock);
		wait_for_cooldown(coder->left_dongle, coder->sim);
		print_status(coder, "has taken a dongle");
	}
	else
	{
		pthread_mutex_lock(&coder->left_dongle->lock);
		wait_for_cooldown(coder->left_dongle, coder->sim);
		print_status(coder, "has taken a dongle");
		pthread_mutex_lock(&coder->right_dongle->lock);
		wait_for_cooldown(coder->right_dongle, coder->sim);
		print_status(coder, "has taken a dongle");
	}
}

void	drop_dongles(t_coder *coder)
{
	long long	now;

	now = get_time_ms();
	coder->left_dongle->available_at = now + coder->sim->dongle_cooldown;
	coder->right_dongle->available_at = now + coder->sim->dongle_cooldown;
	pthread_mutex_unlock(&coder->left_dongle->lock);
	pthread_mutex_unlock(&coder->right_dongle->lock);
}