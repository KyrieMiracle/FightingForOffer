#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <iostream>

using namespace std;

#define DEFAULT_TIME 10
#define MIN_WAIT_TASK_NUM 10
#define DEFAULT_THREAD_VARY 10
#define true 1
#define false 0

typedef struct{
    void *(*function)(void *);
    void *arg;
} threadpool_task_t;

struct threadpool_t{
    pthread_mutex_t lock;
    pthread_mutex_t thread_counter;
    pthread_cond_t queue_not_full;
    pthread_cond_t queue_not_empty;

    pthread_t *threads;
    pthread_t adjust_tid;
    threadpool_task_t *task_queue;

    int min_thr_num;
    int max_thr_num;
    int live_thr_num;
    int busy_thr_num;
    int wait_exit_thr_num;

    int queue_front;
    int queue_rear;
    int queue_size;
    int queue_max_size;

    int shutdown;
};

void *threadpool_thread(void *threadpool);

void *adjust_thread(void *threadpool);

int is_thread_alive(pthread_t tid);

int threadpool_free(threadpool_t *pool);

threadpool_t *threadpool_create(int min_thr_num, int max_thr_num, int queue_max_size){
    threadpool_t *pool = nullptr;
    do{
        if((pool = new threadpool_t) == nullptr){
            cout<<"new threadpool fail"<<endl;
            break;
        }

        pool->min_thr_num = min_thr_num;
        pool->max_thr_num = max_thr_num;
        pool->busy_thr_num = 0;
        pool->live_thr_num = min_thr_num;
        pool->queue_size = 0;
        pool->queue_max_size = queue_max_size;
        pool->queue_front = 0;
        pool->queue_rear = 0;
        pool->shutdown = false;

        pool->threads = (pthread_t *)malloc(sizeof(pthread_t)*max_thr_num);
        if(pool->threads == nullptr){
            cout<<"malloc threads fail"<<endl;
            break;
        }
        memset(pool->threads, 0, sizeof(pthread_t)*max_thr_num);

        pool->task_queue = (threadpool_task_t *)malloc(sizeof(threadpool_task_t)*queue_max_size);
        if(pool->task_queue == nullptr){
            cout<<"malloc task_queue fail"<<endl;
            break;
        }

        if(pthread_mutex_init(&(pool->lock), NULL) != 0
                || pthread_mutex_init(&(pool->thread_counter), NULL) != 0
                || pthread_cond_init(&(pool->queue_not_full), NULL) != 0
                || pthread_cond_init(&(pool->queue_not_empty), NULL) != 0 )
        {
            cout<<"init the lock or cond fail"<<endl;
            break;
        }

        for(int i = 0; i < min_thr_num; i++){
            pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void *)pool);
            cout<<"start thread 0x"<<(unsigned int)pool->threads[i]<<endl;
        }
        pthread_create(&(pool->adjust_tid), NULL, adjust_thread, (void *)pool);

        return pool;

    }while(0);

    threadpool_free(pool);

    return nullptr;
}

int threadpool_add(threadpool_t *pool, void*(*function)(void *arg), void *arg){
    pthread_mutex_lock(&(pool->lock));

    while((pool->queue_size == pool->queue_max_size) && (!pool->shutdown)){
        pthread_cond_wait(&(pool->queue_not_full), &(pool->lock));
    }
    if(pool->shutdown){
        pthread_mutex_unlock(&(pool->lock));
    }

    if(pool->task_queue[pool->queue_rear].arg != nullptr){
        free(pool->task_queue[pool->queue_rear].arg);
        pool->task_queue[pool->queue_rear].arg = nullptr;
    }

    pool->task_queue[pool->queue_rear].function = function;
    pool->task_queue[pool->queue_rear].arg = arg;
    pool->queue_rear = (pool->queue_rear + 1) % pool->queue_max_size;
    pool->queue_size++;

    pthread_cond_signal(&(pool->queue_not_empty));

    pthread_mutex_unlock(&(pool->lock));

    return 0;
}

void *threadpool_thread(void *threadpool){


    threadpool_t *pool = (threadpool_t *)threadpool;
    threadpool_task_t task;

    while(1){
        pthread_mutex_lock(&(pool->lock));

        while((pool->queue_size == 0) && (!pool->shutdown)){
            cout<<"thread 0x"<<(unsigned int)pthread_self()<<" is waiting"<<endl;
            pthread_cond_wait(&(pool->queue_not_empty), &(pool->lock));

            if(pool->wait_exit_thr_num > 0){
                pool->wait_exit_thr_num--;

                if(pool->live_thr_num > pool->min_thr_num){
                    cout<<"thread 0x"<<(unsigned int)pthread_self()<<" is exiting"<<endl;
                    pool->live_thr_num--;
                    pthread_mutex_unlock(&(pool->lock));
                    pthread_exit(NULL);
                }
            }
        }

        if(pool->shutdown){
            pthread_mutex_unlock(&(pool->lock));
            cout<<"thread 0x"<<(unsigned int)pthread_self()<<" is exiting"<<endl;
            pthread_exit(NULL);
        }

        task.function = pool->task_queue[pool->queue_front].function;
        task.arg = pool->task_queue[pool->queue_front].arg;

        pool->queue_front = (pool->queue_front + 1) % pool->queue_max_size;
        pool->queue_size--;

        pthread_cond_broadcast(&(pool->queue_not_full));

        pthread_mutex_unlock(&(pool->lock));

        cout<<"thread 0x"<<(unsigned int)pthread_self()<<" start working"<<endl;
        pthread_mutex_lock(&(pool->thread_counter));
        pool->busy_thr_num++;
        pthread_mutex_unlock(&(pool->thread_counter));
        (*(task.function))(task.arg);

        cout<<"thread 0x"<<(unsigned int)pthread_self()<<" end working"<<endl;
        pthread_mutex_lock(&(pool->thread_counter));
        pool->busy_thr_num--;
        pthread_mutex_unlock(&(pool->thread_counter));
    }
    pthread_exit(NULL);
}

void *adjust_thread(void *threadpool){
    threadpool_t *pool = (threadpool_t *)threadpool;
    while(!pool->shutdown){
        sleep(DEFAULT_TIME);

        pthread_mutex_lock(&(pool->lock));
        int queue_size = pool->queue_size;
        int live_thr_num = pool->live_thr_num;
        pthread_mutex_unlock(&(pool->lock));

        pthread_mutex_lock(&(pool->thread_counter));
        int busy_thr_num = pool->busy_thr_num;
        pthread_mutex_unlock(&(pool->thread_counter));

        if(queue_size >= MIN_WAIT_TASK_NUM && live_thr_num < pool->max_thr_num){
            pthread_mutex_lock(&(pool->lock));
            int add = 0;
            for(int i = 0; i < pool->max_thr_num && add < DEFAULT_THREAD_VARY && pool->live_thr_num < pool->max_thr_num; i++){
                if(pool->threads[i] == 0 || !is_thread_alive(pool->threads[i])){
                    pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void*)pool);
                    add++;
                    pool->live_thr_num++;                    
                }
            }
            pthread_mutex_unlock(&(pool->lock));

        }

        if((busy_thr_num * 2) < live_thr_num && live_thr_num > pool->min_thr_num){
            pthread_mutex_lock(&(pool->lock));
            pool->wait_exit_thr_num = DEFAULT_THREAD_VARY;
            pthread_mutex_unlock(&(pool->lock));

            for(int i = 0; i < DEFAULT_THREAD_VARY; i++){
                pthread_cond_signal(&(pool->queue_not_empty));
            }
        }
    }
    return nullptr;
}

int threadpool_destroy(threadpool_t *pool){
    if(pool == nullptr){
        return -1;
    }
    pool->shutdown = true;

    pthread_join(pool->adjust_tid, NULL);

    for(int i = 0; i < pool->live_thr_num; i++){
        pthread_cond_broadcast(&(pool->queue_not_empty));
    }
    for(int i = 0; i < pool->live_thr_num; i++){
        pthread_join(pool->threads[i], NULL);
    }
    threadpool_free(pool);

    return 0;
}

int threadpool_free(threadpool_t *pool){
    if(pool == nullptr){
        return -1;
    }
    if(pool->task_queue){
        free(pool->task_queue);
    }
    if(pool->threads){
        free(pool->threads);
        pthread_mutex_lock(&(pool->lock));
        pthread_mutex_destroy(&(pool->lock));
        pthread_mutex_lock(&(pool->thread_counter));
        pthread_mutex_destroy(&(pool->thread_counter));
        pthread_cond_destroy(&(pool->queue_not_empty));
        pthread_cond_destroy(&(pool->queue_not_full));
    }
    free(pool);
    pool = nullptr;
    return 0;
}

int threadpool_all_threadnum(threadpool_t *pool){
    int all_thread_num = -1;
    pthread_mutex_lock(&(pool->lock));
    all_thread_num = pool->live_thr_num;
    pthread_mutex_unlock(&(pool->lock));
    return all_thread_num;
}

int threadpool_busy_threadnum(threadpool_t *pool){
    int busy_thread_num = -1;
    pthread_mutex_lock(&(pool->thread_counter));
    busy_thread_num = pool->busy_thr_num;
    pthread_mutex_unlock(&(pool->thread_counter));
    return busy_thread_num;
}

int is_thread_alive(pthread_t tid){
    int kill_rc = pthread_kill(tid, 0);
    if(kill_rc == ESRCH){
        return false;
    }    
    return true;
}

void *process(void *arg){
    cout<<"thread "<<(unsigned int)pthread_self()<<" working on task "<<*(int*)arg<<endl;
    sleep(1);
    cout<<"task "<<*(int*)arg<<" is end"<<endl;
    return nullptr;
}

int main()
{
    threadpool_t *thp = threadpool_create(3,100,100);
    cout<<"pool initialized"<<endl;

    int num[20];
    for(int i = 0; i < 20; i++){
        num[i] = i;
        cout<<"add task "<<i<<endl;
        threadpool_add(thp, process, (void*)(&num[i]));
    }
    sleep(10);
    threadpool_destroy(thp);

    return 0;
}































