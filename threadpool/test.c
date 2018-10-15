#include "threadpool.h"

void *mytask( void *arg)
{
	int id = *(int*)arg;
	free(arg);
	printf("%#X thread execute %d\n", pthread_self(), id);
	sleep(1);
}

int main( void )
{
	threadpool_t pool;

	threadpool_init(&pool, 4);

	for (int i=0; i<10; i++) {
		int *p = malloc(sizeof(int));
		*p = i;
		threadpool_add(&pool, mytask, p);
	}

	threadpool_destroy(&pool);
}

