#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <string.h>

# define SIM_FIFO 1
# define SIM_EDF 2

typedef struct s_sim	t_sim;
typedef struct s_coder	t_coder;

typedef struct s_pq_node
{
	t_coder		*coder;
	long long	key;
}	t_pq_node;

typedef struct s_pqueue
{
	t_pq_node	nodes[2];
	int			size;
}	t_pqueue;

typedef struct s_dongle
{
	pthread_mutex_t	lock;
	long long		available_at;
	int				taken;
	t_pqueue		pq;
}	t_dongle;

struct s_sim
{
	int				nb_coders;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	int				nb_compiles_req;
	long long		dongle_cooldown;
	int				scheduler;
	long long		start_time;
	int				stop_flag;
	pthread_mutex_t	print_lock;
	pthread_mutex_t	sim_lock;
	t_dongle		*dongles;
};

struct s_coder
{
	int			id;
	long long	last_compile_start;
	int			compiles_done;
	t_dongle	*left_dongle;
	t_dongle	*right_dongle;
	t_coder		*left_neighbor;
	t_coder		*right_neighbor;
	t_sim		*sim;
};

// init.c
int			init_sim(t_sim *sim, t_coder **coders);
int			init_dongles(t_sim *sim);
t_coder		*init_coders(t_sim *sim);

// routine.c
int			take_dongles(t_coder *coder);
void		drop_dongles(t_coder *coder);
int			request_dongle(t_coder *coder, t_dongle *dongle);

// routine_helper.c
void		*coder_routine(void *arg);
int			check_stop_or_single(t_coder *coder);

// monitor.c
void		*monitor_routine(void *arg);

// utils.c
long long	get_time_ms(void);
void		custom_sleep(long long time_to_sleep, t_sim *sim);
void		print_status(t_coder *coder, char *status);
int			is_numeric(char *str);
void		clean_sim(t_sim *sim, t_coder *coders);

// queue.c
void		pq_push(t_pqueue *pq, t_coder *coder, long long key);
void		pq_pop(t_pqueue *pq);
void		ft_swap(t_pq_node *a, t_pq_node *b);

#endif