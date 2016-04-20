/*
*   Author		Chenfuming
*   Email		flemingchan@outlook.com
*   File Name	thread_pool.c
*	Desc		线程池的实现
*   Date 		2016.04.15
*/

#include "thread_pool.h"

static CThread_pool *pool = NULL;

void thread_pool_init(int max_thread_num)
{
	pool = (CThread_pool *)malloc(sizeof(CThread_pool));
	/*初始化互斥锁和条件变量*/
	pthread_mutex_init(&(pool->queue_lock), NULL);
	pthread_cond_init(&(pool->queue_ready), NULL);
	/*初始化其他参数*/
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
	/*新建工作任务节点*/
	CThread_worker *newworker = (CThread_worker *)malloc(sizeof(CThread_worker));
	newworker->process = process;
	newworker->arg = arg;
	newworker->next = NULL;
	
	pthread_mutex_lock(&(pool->queue_lock));
	
	CThread_worker *temp_queue = pool->queue_head;

	if (NULL != pool->queue_head)
	{
		while (NULL != temp_queue)/*把新建的任务节点插入到队列尾*/
		{
			temp_queue = temp_queue->next;
		}
		temp_queue = newworker;
	}
	else /*队列为空时，任务节点放在队头*/
	{
		pool->queue_head = newworker;
	}
	pool->cur_queue_size++;
	
	pthread_mutex_unlock(&(pool->queue_lock));
	
	/*工作任务队列中已有任务，唤醒一个线程去工作*/
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
         *如果等待队列为空，进入阻塞状态(等待条件变量唤醒);等待前会解锁，唤醒后会加锁
		 *若工作队列已有任务，无需再等待唤醒，直接读取工作任务	
		 */
        while (pool->cur_queue_size == 0 && !pool->shutdown)
        {
            pthread_cond_wait(&(pool->queue_ready), &(pool->queue_lock));
        }
        /*线程池要销毁了*/
        if (pool->shutdown)
        {
            pthread_mutex_unlock(&(pool->queue_lock));
            pthread_exit(NULL);
        }

        assert(pool->cur_queue_size != 0);
        assert(pool->queue_head != NULL);
		
		/*取出并删除工作任务队列的最前面的任务*/
		worker = pool->queue_head;
		pool->queue_head = pool->queue_head->next;
		(pool->cur_queue_size)--;
		pthread_mutex_unlock(&(pool->queue_lock));

		/*处理业务*/		
		(*(worker->process))(worker->arg);
    	free(worker);
    	worker = NULL;

	}

    pthread_exit(NULL);
}

/*销毁线程池，等待队列中的任务不会再被执行，但是正在运行的线程会一直
把任务运行完后再退出*/
int thread_pool_destroy()
{
	if (pool->shutdown)/*防止两次调用*/
    {
        return -1;
    }
    
    pool->shutdown = 1;
    /*唤醒所有等待线程，线程池要销毁了*/
    pthread_cond_broadcast(&(pool->queue_ready));
    /*阻塞等待线程退出，否则就成僵尸了*/
    int i;
    for (i = 0; i < pool->max_thread_num; i++)
    {
        pthread_join(pool->threadid[i], NULL);
    }
    
    free(pool->threadid);
    /*销毁等待队列*/
    CThread_worker *head = NULL;
    while (pool->queue_head != NULL)
    {
        head = pool->queue_head;
        pool->queue_head = pool->queue_head->next;
        free (head);
    }
    /*销毁条件变量和互斥量*/
    pthread_mutex_destroy(&(pool->queue_lock));
    pthread_cond_destroy(&(pool->queue_ready));
    
    free(pool);
    pool = NULL;
    return 0;
}

