/*
*   Author		Chenfuming
*   Email		flemingchan@outlook.com
*   File Name	thread_pool.h
*   Date 		2016.04.15
*/
 
#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>

#define THREAD_POLL_SIZE 10 //�̳߳صĴ�С

/*
*�̳߳����������к͵ȴ���������һ��CThread_worker
*�������������������������һ������ṹ
*/
typedef struct worker
{
    /*�ص���������������ʱ����ô˺�����ע��Ҳ��������������ʽ*/
    void *(*process) (void *arg);
    void *arg;/*�ص������Ĳ���*/
    struct worker *next;
} CThread_worker;

/*�̳߳ؽṹ*/
typedef struct
{
    pthread_mutex_t queue_lock;
    pthread_cond_t queue_ready;
    /*����ṹ���̳߳������еȴ�����*/
    CThread_worker *queue_head;
    /*�Ƿ������̳߳�*/
    int shutdown;
    pthread_t *threadid;
    /*�̳߳�������Ļ�߳���Ŀ*/
    int max_thread_num;
    /*��ǰ�ȴ����е�������Ŀ*/
    int cur_queue_size;
} CThread_pool;

void thread_pool_init(int max_thread_num);
int pool_add_worker(void *(*process)(void *arg), void *arg);
void *thread_routine(void *arg);
int thread_pool_destroy();

#endif

