#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>


typedef struct s_god
{
	pthread_mutex_t	mutex;
	unsigned long	base;
	int	*current;
	int	i;
	int	max;
}			t_god;

unsigned long get_time(void)
{
	struct timeval	base;
	gettimeofday(&base, NULL);
	return ((base.tv_sec * 1000 ) + (base.tv_usec / 1000));
}

void	*thread_run(void *arg)
{
	t_god *temp = (t_god *)arg;
	pthread_mutex_lock(&temp->mutex);
	printf("philo %d eat at time : %lu\n", temp->current[temp->i], (get_time() - temp->base));
	temp->i++;
	pthread_mutex_unlock(&temp->mutex);
	return (arg);
}

void	add_value(int *arr)
{
	int	i;
	i = 0;
	while (i < 6)
	{
		arr[i] = i;
		i++;
	}
}

void	start_thread(int thread)
{
	pthread_t	arr[thread];
	t_god		hi;

	hi.base = get_time();
	hi.i = 0;
	hi.max = 6;
	hi.current = (int *)malloc(sizeof(int) * 6);
	add_value(hi.current);
	printf("--%lu--\n", hi.base);
	pthread_mutex_init(&hi.mutex, NULL);
	for (int i = 0; i < thread; i++)
	{
		pthread_create(&arr[i], NULL, &thread_run, &hi);
	}
	for (int i = 0; i < thread; i++)
	{
		pthread_join(arr[i], NULL);
	}
}

int	main(int ac, char *av[])
{
	start_thread(atoi(av[1]));
	return (0);
}
