#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

// struct  -> child struct 
// struct that send to pthread_create should contain : mutex, base ,numbers of philo (this is variious point)
// send iterate num and send array of int arr[] and iterate it in mutex
//
// case of 5; how 5 can eat first in 3rd round;
// // track how many time that philo ate;
// // // logic : if (n_round % 2 != 0) while (t_data-> ate_n_time < n_round / 2) then start eat;
// case 2 : 2 need to get (2 fork and 2 + 1 % n_philo); philo_mutex_lock fork and unlock when finish eat


// args : number_of_philosophers , time_to_die , time_to_eat , time_to_sleep , [number_of_times_each_philosopher_must_eat]

// all time is millisec;

// ft_atoi =======

int	ft_atoi_no_protect(char *s)
{
	long res;

	res = 0;
	while (*s && *s >= '0' && *s <= '9')
		res = (res * 10) + (*s++ - '0');
	return ((int)res);
}

unsigned long	ft_atoul_no_protect(char *s)
{
	unsigned long res;

	res = 0;
	while (*s && *s >= '0' && *s <= '9')
		res = (res * 10) + (*s++ - '0');
	return (res);
}

// ===============

typedef struct	s_env
{
	int	philo_size;
	int	max_eat;
	unsigned long time_to_die;
	unsigned long time_to_eat;
	unsigned long time_to_sleep;

	unsigned long base_time;

	pthread_t	*philo;
	pthread_mutex_t init_id;
	pthread_mutex_t check_fork;
	int	*fork;
	int	philo_id_iter;

	int dead_found;
	int *cycle_end;
	int	*dead_arr;
}		t_env;

int	check_input(char *s)
{
	if (!*s)
		return (2); // input_error
	while (*s)
	{
		if (*s >= '0' && *s <= '9')
			s++;
		else
			break;
	}
	if (*s)
		return (2); // success exit;
	return (0);
}

int	valid_input(t_env *env)
{
	if (env->philo_size == 0 || env->time_to_die == 0
		||env->time_to_eat == 0 || env->time_to_sleep == 0)
		return (2); // input value error
	if (env->max_eat == 0)
		return (2); // input value error
	return (0);
}

int	get_input(char *arr[], int ac, t_env *env)
{
	if (check_input(arr[1]) || check_input(arr[2])
		|| check_input(arr[3]) || check_input(arr[4]))
		return (2); // bad input
	if (ac == 6)
	{
		if (check_input(arr[5]))
			return (2); // bad input
		env->max_eat = ft_atoi_no_protect(arr[5]);
	}
	else
		env->max_eat = -1; // no optaional for arr[5]
	env->philo_size = ft_atoi_no_protect(arr[1]);
	env->time_to_die = ft_atoul_no_protect(arr[2]);
	env->time_to_eat = ft_atoul_no_protect(arr[3]);
	env->time_to_sleep = ft_atoul_no_protect(arr[4]);
	if (valid_input(env))
		return (2); //input value error
	return (0);
}

// ===== get_time ====

unsigned long	get_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

unsigned long	get_dif_time(unsigned long time)
{
	long temp;

	temp = (long)get_time() - time;
	if (temp < 0)
		return (temp * -1);
	return (temp);
}

//  =========  start thread ======

void	*template(void *arg)
{
	int	philo_id;
	t_env	*env;
	int dead;
	int ate;
	int	wait;
	unsigned long end_time;

	wait = 1;
	dead = 0;
	ate = 0;
	env = (t_env *)arg;
	pthread_mutex_lock(&env->init_id);
	philo_id = env->philo_id_iter;
	printf("get_id : %d\n", philo_id);
	env->philo_id_iter++;
	pthread_mutex_unlock(&env->init_id);
	while (env->dead_found == 0 && env->cycle_end[philo_id - 1]) 
	{
		pthread_mutex_lock(&env->check_fork);
		// printf("mutex_lock : %d\n", philo_id);
		if (env->fork[philo_id - 1] == 1 && env->fork[philo_id % env->philo_size] == 1)
		{
			env->fork[philo_id -1 ] = 0;
			env->fork[philo_id % env->philo_size] = 0;
			env->cycle_end[philo_id - 1] -= 1;
			if (env->time_to_eat > get_dif_time(env->dead_arr[philo_id - 1]))
			{
				dead = 1;
				env->dead_found = 1;
			}
			pthread_mutex_unlock(&env->check_fork);
			ate = 1;
			wait = 0;
			printf("%lu %d has taken a fork\n", get_dif_time(env->base_time),  philo_id);
			printf("%lu %d has taken a fork\n", get_dif_time(env->base_time),  philo_id);
			printf("%lu %d is eating\n", get_dif_time(env->base_time), philo_id);
			printf("%lu time to sleep \n", env->time_to_sleep);
			printf("%lu time to die \n", get_dif_time(env->dead_arr[philo_id - 1]));  // need to check dead_arr sould be the one who got minus from current gettimeofday() :TODO:
			if (dead)
			{
				usleep(get_dif_time(env->dead_arr[philo_id - 1]) * 1000);  // need to check dead_arr sould be the one who got minus from current gettimeofday() :TODO:
				printf("%d has dead\n", philo_id);
			}
			else
				usleep(env->time_to_sleep * 1000);
			pthread_mutex_lock(&env->check_fork);
			env->fork[philo_id - 1] = 1;
			env->fork[philo_id % env->philo_size] = 1;
			pthread_mutex_unlock(&env->check_fork);
		}
		else if (wait == 1)
		{
			pthread_mutex_unlock(&env->check_fork);
			printf("%lu %d is waiting\n",get_dif_time(env->base_time), philo_id);
			wait = 0;
		}
		else
		{
			pthread_mutex_unlock(&env->check_fork);
			usleep(100);
		}
		if (ate == 1)
		{
			printf("%lu %d is sleeping\n", get_dif_time(env->base_time),  philo_id);
			usleep(env->time_to_sleep * 1000);
			printf("%lu %d is thinking\n", get_dif_time(env->base_time),  philo_id);
			ate = 0;
			wait = 1;
		}
	}
	return (arg);
}

void init_env(t_env *env)
{
	int	i;

	env->philo = (pthread_t *)malloc(sizeof(pthread_t) * env->philo_size);
	env->fork = (int *)malloc(sizeof(int) * env->philo_size);
	env->cycle_end = (int *)malloc(sizeof(int) * env->philo_size);
	env->dead_arr = (int *)malloc(sizeof(int) * env->philo_size);
	env->dead_found = 0;
	env->philo_id_iter = 1;
	pthread_mutex_init(&env->init_id, NULL);
	pthread_mutex_init(&env->check_fork, NULL);
    env->base_time = get_time(); 
	i = 0;
	while (i < env->philo_size)
	{
		env->fork[i] = 1;
		env->cycle_end[i] = env->max_eat;
		env->dead_arr[i] = get_time() + env->time_to_die;
		i++;
	}
}

int	start_thread(t_env *env)
{
	int	i;

	init_env(env);
	i = 0;
	while (i < env->philo_size)
	{
		pthread_create(&env->philo[i], NULL, &template, env);
		i++;
	}
	i = 0;
	while (i < env->philo_size)
	{
		pthread_join(env->philo[i], NULL);
		i++;
	}
	pthread_mutex_destroy(&env->init_id);
	pthread_mutex_destroy(&env->check_fork);
	return (0);
}


// ==============
int	main(int ac, char *av[])
{
	t_env	env;

	if (ac < 5 || ac > 6)
		return (2); // arg_error
	if (get_input(av, ac, &env) != 0)
		return (2); // input_error
	printf("end\n");
	if (start_thread(&env) != 0)
		return (2); // error_run;
	return (0);
}
