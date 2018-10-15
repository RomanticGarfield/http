#include "condition.h"

int condition_init(condition_t *pc)
{
	int ret = 0;

	do {
		ret = pthread_cond_init(&pc->pcond, NULL);
		if (ret != 0 ) break;
		ret = pthread_mutex_init(&pc->pmutex, NULL);
	} while ( 0 );

	return ret;
}

int condition_destroy(condition_t *pc)
{
	int ret = 0;
	do {
		ret = pthread_cond_destroy(&pc->pcond);
		if (ret != 0 ) break;
		ret = pthread_mutex_destroy(&pc->pmutex);
	} while ( 0 );

	return 0;
}

int condition_lock(condition_t *pc)
{
	return pthread_mutex_lock(&pc->pmutex);
}

int condition_unlock(condition_t *pc)
{
	return pthread_mutex_unlock(&pc->pmutex);
}

int condition_wait(condition_t *pc)
{
	return pthread_cond_wait(&pc->pcond, &pc->pmutex);
}

int condition_timedwait(condition_t *pc, struct timespec *abstime)
{
	return pthread_cond_timedwait(&pc->pcond, &pc->pmutex, abstime);
}

int condition_signal(condition_t *pc)
{
	return pthread_cond_signal(&pc->pcond);
}

int condition_boardcast(condition_t *pc)
{
	return pthread_cond_broadcast(&pc->pcond);
}

