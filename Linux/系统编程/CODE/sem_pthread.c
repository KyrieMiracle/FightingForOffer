#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


/*
信号量理解：
1、线程能够并行访问共享空间
2、可以设置并行的数量
3、可用于线程，也可用于进程

信号量和互斥锁(mutex)的区别：互斥锁只允许一个线程进入临界区，而信号量允许多个线程同时进入临界区。

不多做解释，要使用信号量同步，需要包含头文件semaphore.h。

主要用到的函数：

int sem_init(sem_t *sem, int pshared, unsigned int value);，其中sem是要初始化的信号量，pshared表示此信号量是在进程间共享还是线程间共享，value是信号量的初始值。
int sem_destroy(sem_t *sem);,其中sem是要销毁的信号量。只有用sem_init初始化的信号量才能用sem_destroy销毁。
int sem_wait(sem_t *sem);等待信号量，如果信号量的值大于0,将信号量的值减1,立即返回。如果信号量的值为0,则线程阻塞。相当于P操作。成功返回0,失败返回-1。
int sem_post(sem_t *sem); 释放信号量，让信号量的值加1。相当于V操作。

*/


/* @Scene: 某行业营业厅同时只能服务两个顾客。
 * 有多个顾客到来，每个顾客如果发现服务窗口已满，就等待，
 * 如果有可用的服务窗口，就接受服务。 */

/* 将信号量定义为全局变量，方便多个线程共享 */
sem_t sem;

/* 每个线程要运行的例程 */
void * get_service(void *thread_id)
{
    /* 注意：立即保存thread_id的值，因为thread_id是对主线程中循环变量i的引用，它可能马上被修改 */
    int customer_id = *((int *)thread_id);

    if(sem_wait(&sem) == 0) {
        usleep(100);                /* service time: 100ms */
        printf("customer %d receive service ...\n", customer_id);
        sem_post(&sem);
    }
}

#define CUSTOMER_NUM 10

int main(int argc, char *argv[])
{
    /* 初始化信号量，初始值为2，表示有两个顾客可以同时接收服务 */
    /* @prototype: int sem_init(sem_t *sem, int pshared, unsigned int value); */
    /* pshared: if pshared == 0, the semaphore is shared among threads of a process
     * otherwise the semaphore is shared between processes.   */
    sem_init(&sem, 0, 2);

    /* 为每个顾客定义一个线程id, pthread_t 其实是unsigned long int */
    pthread_t customers[CUSTOMER_NUM];

    int i, ret;
    /* 为每个顾客生成一个线程 */
    for(i = 0; i < CUSTOMER_NUM; i++){
        int customer_id = i;
        ret = pthread_create(&customers[i], NULL, get_service, &customer_id);
        if(ret != 0){
            perror("pthread_create");
            exit(1);
        }
        else {
            printf("Customer %d arrived.\n", i);
        }
        usleep(10);
    }

    /* 等待所有顾客的线程结束 */
    /* 注意：这地方不能再用i做循环变量，因为可能线程中正在访问i的值 */
    int j;
    for(j = 0; j < CUSTOMER_NUM; j++) {
        pthread_join(customers[j], NULL);
    }

    /* Only a  semaphore that  has been initialized  by sem_init(3)
     * should be destroyed using sem_destroy().*/
    sem_destroy(&sem);
    return 0;
}