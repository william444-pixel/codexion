#include "../include/codexion.h"

int	init_dongles(t_sim *sim)
{
	int	i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->nb_coders);
	if (!sim->dongles)
		return (1);
	i = 0;
	while (i < sim->nb_coders)
	{
		if (pthread_mutex_init(&sim->dongles[i].lock, NULL) != 0)
			return (1);
		sim->dongles[i].available_at = 0;
		sim->dongles[i].taken = 0;
		sim->dongles[i].pq.size = 0;
		i++;
	}
	return (0);
}

t_coder	*init_coders(t_sim *sim)
{
	t_coder	*coders;
	int		i;

	coders = malloc(sizeof(t_coder) * sim->nb_coders);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < sim->nb_coders)
	{
		coders[i].id = i + 1;
		coders[i].compiles_done = 0;
		coders[i].sim = sim;
		coders[i].left_dongle = &sim->dongles[i];
		coders[i].right_dongle = &sim->dongles[(i + 1) % sim->nb_coders];
		coders[i].left_neighbor = &coders[(i + sim->nb_coders - 1) % sim->nb_coders];
		coders[i].right_neighbor = &coders[(i + 1) % sim->nb_coders];
		i++;
	}
	return (coders);
}

int	init_sim(t_sim *sim, t_coder **coders)
{
	sim->stop_flag = 0;
	if (pthread_mutex_init(&sim->print_lock, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->sim_lock, NULL) != 0)
		return (1);
	if (init_dongles(sim) != 0)
		return (1);
	*coders = init_coders(sim);
	if (!*coders)
		return (1);
	return (0);
}