#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "condition.h"

// 任务节点
typedef struct node {
	void *(*pfun)(void *); // 回调函数
	void *arg; // 回调函数的参数
	struct node *next;
}node_t;

// 代表线程池
typedef struct threadpool {
	condition_t ready;      // 控制同步和互斥
	node_t *first;          // 队头指针
	node_t *tail;           // 队尾指针
	int counter;            // 线程池中当前有多少个线程 
	int idle;               // 空闲线程个数
	int max_thread;         // 最多可以创建多少个线程
	int quit;               // 为1,代表要销毁线程池
}threadpool_t;

// 初始化线程池
void threadpool_init(threadpool_t *pool, int max_thread);

// 添加任务
void threadpool_add(threadpool_t *pool, void *(*pf)(void*), void *arg);

// 销毁线程池
void threadpool_destroy(threadpool_t *pool);

#endif //  __THREADPOOL_H__

