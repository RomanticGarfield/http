#include "threadpool.h"
#include <errno.h>

// 初始化线程池
void threadpool_init(threadpool_t *pool, int max_thread)
{
	condition_init(&pool->ready);
	pool->tail       = NULL;
	pool->idle       = 0;
	pool->quit       = 0;
	pool->first      = NULL;
	pool->counter    = 0;
	pool->max_thread = max_thread;
}

static void *route(void *arg)
{
	threadpool_t *pool = (threadpool_t*)arg;
	int timeout = 0;

	while ( 1 ) {
		timeout = 0;
		condition_lock(&pool->ready);		
		pool->idle++; // 还没开始工作，是一个空闲线程
		while ( pool->first == NULL && pool->quit==0) {
			struct timespec abs;
			clock_gettime(CLOCK_REALTIME,  &abs);
			abs.tv_sec += 2;
			int ret = condition_timedwait(&pool->ready, &abs);
			if ( ret == ETIMEDOUT ) {
				timeout = 1;
				break;
			}
		}
		pool->idle--; // 开始工作，空闲线程个数减少

		// 处理任务
		if ( pool->first != NULL ) {
			node_t *p = pool->first;
			pool->first = p->next;
			// 防止回调任务执行时间太长，导致别的线程没法执行
			condition_unlock(&pool->ready);		
			(p->pfun)(p->arg);
			free(p);
			condition_lock(&pool->ready);		
		}

		// 处理超时
		if ( timeout == 1 && pool->first == NULL ) {
			printf("%#X thread time out exit\n", pthread_self());
			pool->counter--;
			condition_unlock(&pool->ready);		
			break;
		}

		// 收到退出通知
		if ( pool->quit == 1 && pool->first == NULL ) {
			printf("%#X thread quit\n", pthread_self());
			pool->counter--;
			if ( pool->counter == 0 )
				condition_signal(&pool->ready);
			condition_unlock(&pool->ready);
			break;
		}

		condition_unlock(&pool->ready);		
	}
}

// 添加任务
void threadpool_add(threadpool_t *pool, void *(*pf)(void*), void *arg)
{
	// 生成任务节点
	node_t *pnode = (node_t*)malloc(sizeof(node_t));
	memset(pnode, 0x00, sizeof(node_t));
	pnode->pfun = pf; 
	pnode->arg  = arg;
	pnode->next = NULL;

	// 修改多个线程都可能修改的数据
	condition_lock(&pool->ready);

	// 放入任务队列
	if ( pool->first == NULL ) 
		pool->first = pnode;
	else
		pool->tail->next = pnode;
	pool->tail = pnode;

	// 如果池中有空闲线程，唤醒它执行任务
	if ( pool->idle > 0 ) {
		condition_signal(&pool->ready);
	} 
	// 如果没有空闲线程，并且当前线程个数小于最大值，创建新线程来执行任务
	else if ( pool->counter < pool->max_thread ) {
		pthread_t tid;
		pthread_create(&tid, NULL, route, (void*)pool);
		pthread_detach(tid);
		pool->counter ++; // 线程池中线程个数增加
	}

	condition_unlock(&pool->ready);
}

// 销毁线程池
void threadpool_destroy(threadpool_t *pool)
{
	if ( pool->quit == 1 )
		return;

	condition_lock(&pool->ready);
	pool->quit = 1;
	if ( pool->counter > 0 ) {
		while ( pool->idle > 0 ) 
			condition_boardcast(&pool->ready);
	}
	
	//  等待没有收到销毁广播,这些线程都是正在执行任务。
	while ( pool->counter > 0 ) {
		condition_wait(&pool->ready);
	}
	
	condition_unlock(&pool->ready);
}

