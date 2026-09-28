#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <string.h>

/* ======================================= */
/*               STRUCTURES                */
/* ======================================= */

typedef enum e_scheduler {
    SIM_FIFO,
    SIM_EDF
} t_scheduler;

typedef struct s_dongle {
    pthread_mutex_t lock;
    long long       available_at;
} t_dongle;

typedef struct s_sim {
    int             nb_coders;
    long long       time_to_burnout;
    long long       time_to_compile;
    long long       time_to_debug;
    long long       time_to_refactor;
    int             nb_compiles_req;
    long long       dongle_cooldown;
    t_scheduler     scheduler;
    
    long long       start_time;
    int             stop_flag; // 1 ila mat chi wahed wla salaw l'compiles
    
    pthread_mutex_t print_lock; // Mutex d l'affichage
    pthread_mutex_t sim_lock;   // Mutex d la protection dyal stop_flag w compiles_done
    
    t_dongle        *dongles;
} t_sim;

typedef struct s_coder {
    int             id;
    long long       last_compile_start;
    int             compiles_done;
    
    t_dongle        *left_dongle;
    t_dongle        *right_dongle;
    t_sim           *sim;
    
    struct s_coder  *left_neighbor;  // Jaru li 3la lisr
    struct s_coder  *right_neighbor; // Jaru li 3la limn
} t_coder;

/* ======================================= */
/*               PROTOTYPES                */
/* ======================================= */

long long   get_time_ms(void);
void        custom_sleep(long long time_to_sleep, t_sim *sim);
void        print_status(t_coder *coder, char *status);
int         init_sim(t_sim *sim, t_coder **coders);
void        *coder_routine(void *arg);
void        *monitor_routine(void *arg);
int     is_numeric(char *str);
void    clean_sim(t_sim *sim, t_coder *coders);
void    take_dongles(t_coder *coder);
void    drop_dongles(t_coder *coder);


#endif