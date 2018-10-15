#ifndef __CONDITION_H__
#define __CONDITION_H__

#include <pthread.h>
#include <time.h>

typedef struct condition{
	pthread_mutex_t pmutex;
	pthread_cond_t pcond;
}condition_t;

int condition_init(condition_t *pc);
int condition_destroy(condition_t *pc);

int condition_lock(condition_t *pc);
int condition_unlock(condition_t *pc);

int condition_wait(condition_t *pc);
int condition_timedwait(condition_t *pc, struct timespec *abstime);

int condition_signal(condition_t *pc);
int condition_boardcast(condition_t *pc);

#endif //__CONDITION_H__

