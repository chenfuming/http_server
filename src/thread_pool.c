/*
*   Author		Chenfuming
*   Email		flemingchan@outlook.com
*   File Name	thread_pool.c
*	Desc		�̳߳ص�ʵ��
*   Date 		2016.04.15
*/

#include "thread_pool.h"

static CThread_pool *pool = NULL;

void thread_pool_init(int max_thread_num)
{
	pool = (CThread_pool *)malloc(sizeof(CThread_pool));
	/*��ʼ������������������*/
	pthread_mutex_init(&(pool->queue_lock), NULL);
	pthread_cond_init(&(pool->queue_ready), NULL);
	/*��ʼ����������*/
	pool->queue_head = NULL;
    pool->max_thread_num = max_thread_num;
    pool->cur_queue_size = 0;
    pool->shutdown = 0;
    pool->threadid = (pthread_t *)malloc(max_thread_num * sizeof(pthread_t));
    
    int i = 0;
    for (i = 0; i < max_thread_num; i++)
    { 
        pthread_create(&(pool->threadid[i]), NULL, thread_routine, NULL);
    }
}

int pool_add_worker(void *(*process)(void *arg), void *arg)
{
	/*�½���������ڵ�*/
	CThread_worker *newworker = (CThread_worker *)malloc(sizeof(CThread_worker));
	newworker->process = process;
	newworker->arg = arg;
	newworker->next = NULL;
	
	pthread_mutex_lock(&(pool->queue_lock));
	
	CThread_worker *temp_queue = pool->queue_head;

	if (NULL != pool->queue_head)
	{
		while (NULL != temp_queue)/*���½�������ڵ���뵽����β*/
		{
			temp_queue = temp_queue->next;
		}
		temp_queue = newworker;
	}
	else /*����Ϊ��ʱ������ڵ���ڶ�ͷ*/
	{
		pool->queue_head = newworker;
	}
	pool->cur_queue_size++;
	
	pthread_mutex_unlock(&(pool->queue_lock));
	
	/*��������������������񣬻���һ���߳�ȥ����*/
	pthread_cond_signal(&(pool->queue_ready));

	return 0;
}

void *thread_routine(void *arg)
{
	CThread_worker *worker = NULL;
	
	while(1)
	{
		pthread_mutex_lock (&(pool->queue_lock));
        /*
         *����ȴ�����Ϊ�գ���������״̬(�ȴ�������������);�ȴ�ǰ����������Ѻ�����
		 *�����������������������ٵȴ����ѣ�ֱ�Ӷ�ȡ��������	
		 */
        while (pool->cur_queue_size == 0 && !pool->shutdown)
        {
            pthread_cond_wait(&(pool->queue_ready), &(pool->queue_lock));
        }
        /*�̳߳�Ҫ������*/
        if (pool->shutdown)
        {
            pthread_mutex_unlock(&(pool->queue_lock));
            pthread_exit(NULL);
        }

        assert(pool->cur_queue_size != 0);
        assert(pool->queue_head != NULL);
		
		/*ȡ����ɾ������������е���ǰ�������*/
		worker = pool->queue_head;
		pool->queue_head = pool->queue_head->next;
		(pool->cur_queue_size)--;
		pthread_mutex_unlock(&(pool->queue_lock));

		/*����ҵ��*/		
		(*(worker->process))(worker->arg);
    	free(worker);
    	worker = NULL;

	}

    pthread_exit(NULL);
}

/*�����̳߳أ��ȴ������е����񲻻��ٱ�ִ�У������������е��̻߳�һֱ
����������������˳�*/
int thread_pool_destroy()
{
	if (pool->shutdown)/*��ֹ���ε���*/
    {
        return -1;
    }
    
    pool->shutdown = 1;
    /*�������еȴ��̣߳��̳߳�Ҫ������*/
    pthread_cond_broadcast(&(pool->queue_ready));
    /*�����ȴ��߳��˳�������ͳɽ�ʬ��*/
    int i;
    for (i = 0; i < pool->max_thread_num; i++)
    {
        pthread_join(pool->threadid[i], NULL);
    }
    
    free(pool->threadid);
    /*���ٵȴ�����*/
    CThread_worker *head = NULL;
    while (pool->queue_head != NULL)
    {
        head = pool->queue_head;
        pool->queue_head = pool->queue_head->next;
        free (head);
    }
    /*�������������ͻ�����*/
    pthread_mutex_destroy(&(pool->queue_lock));
    pthread_cond_destroy(&(pool->queue_ready));
    
    free(pool);
    pool = NULL;
    return 0;
}

