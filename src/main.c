/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nael-oua <nael-oua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 16:47:02 by nael-oua          #+#    #+#             */
/*   Updated: 2026/09/30 16:53:32 by nael-oua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	check_args_numeric(char **argv, t_sim *sim)
{
	int	i;

	i = 1;
	while (i <= 7)
	{
		if (!is_numeric(argv[i]))
		{
			printf("Error: Arguments must be positive integers.\n");
			return (1);
		}
		i++;
	}
	if (sim->nb_compiles_req < 1)
	{
		printf("number of compiles must be at least 1\n");
		return (1);
	}
	return (0);
}

int	init_args_values(char **argv, t_sim *sim)
{
	sim->nb_coders = atoi(argv[1]);
	sim->time_to_burnout = atoi(argv[2]);
	sim->time_to_compile = atoi(argv[3]);
	sim->time_to_debug = atoi(argv[4]);
	sim->time_to_refactor = atoi(argv[5]);
	sim->nb_compiles_req = atoi(argv[6]);
	sim->dongle_cooldown = atoi(argv[7]);
	if (strcmp(argv[8], "fifo") == 0)
		sim->scheduler = SIM_FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		sim->scheduler = SIM_EDF;
	else
	{
		printf("Error: Scheduler must be 'fifo' or 'edf'.\n");
		return (1);
	}
	if (sim->nb_coders <= 0)
	{
		printf("Error: Number of coders must be more than 0.\n");
		return (1);
	}
	return (0);
}

int	parse_arguments(int argc, char **argv, t_sim *sim)
{
	if (argc != 9)
	{
		printf("Error: Invalid number of arguments.\n");
		return (1);
	}
	if (check_args_numeric(argv, sim))
		return (1);
	return (init_args_values(argv, sim));
}

int	start_simulation(t_sim *sim, t_coder *coders)
{
	int			i;
	pthread_t	monitor_th;
	pthread_t	*coder_th;

	coder_th = malloc(sizeof(pthread_t) * sim->nb_coders);
	if (!coder_th)
		return (1);
	sim->start_time = get_time_ms();
	i = -1;
	while (++i < sim->nb_coders)
	{
		coders[i].last_compile_start = sim->start_time;
		if (pthread_create(&coder_th[i], NULL, \
			(void *)coder_routine, &coders[i]))
			return (1);
	}
	if (pthread_create(&monitor_th, NULL, (void *)monitor_routine, coders))
		return (1);
	pthread_join(monitor_th, NULL);
	i = -1;
	while (++i < sim->nb_coders)
		pthread_join(coder_th[i], NULL);
	free(coder_th);
	return (0);
}

int	main(int argc, char **argv)
{
	t_sim	sim;
	t_coder	*coders;

	memset(&sim, 0, sizeof(t_sim));
	if (parse_arguments(argc, argv, &sim))
		return (1);
	if (init_sim(&sim, &coders) != 0)
	{
		printf("Error: Initialization failed.\n");
		return (1);
	}
	if (start_simulation(&sim, coders) != 0)
	{
		printf("Error: Thread creation failed.\n");
		clean_sim(&sim, coders);
		return (1);
	}
	clean_sim(&sim, coders);
	return (0);
}
