#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

void err_thread(int ret, char *str){
	if(ret != 0){
		fprintf(stderr, "%s:%s\n", str, strerror(ret));
		pthread_exit(NULL);
	}
}


struct msg
{
	int num;
	struct msg *next;
};

struct msg *head;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t has_data = PTHREAD_COND_INITIALIZER;


void* produser(void*){
	while(1){
		struct msg *mp = malloc(sizeof(struct msg));
		mp->num = rand()%1000 + 1;      //模拟生产数据
		printf("-----produser: %d\n", mp->num);
		pthread_mutex_lock(&mutex);     //加锁
		mp->next = head;                //写公共区域数据
		head = mp;
		pthread_mutex_unlock(&mutex);   //解锁

		pthread_cond_signal(&has_data);	//唤醒阻塞在条件变量 has_data 上的线程

		sleep(rand()%3);
	}
	return NULL;
}

void* consumer(void*){
	while(1){
		struct msg *mp;
		pthread_mutex_lock(&mutex);               //加锁
		if(head == NULL){
			pthread_cond_wait(&has_data, &mutex); //阻塞等待条件变量， 解锁
		}                                         //pthread_cond_wait 返回时，重写加锁mutex
		mp = head;
		head = mp->next;
		
		pthread_mutex_unlock(&mutex);             //解锁
		printf("----consumer id: %lu :%d\n",pthread_self(), mp->num);
		free(mp);

		sleep(rand()%3);
    }
	return NULL
}

int main(int argc, char *argv[]){
	int ret; 
	pthread_t pid, cid; 

	srand(time(NULL));

	ret = pthread_create(&pid, NULL, produser, NULL);
	err_thread(ret, "pthread_create error");

	ret = pthread_create(&cid, NULL, consumer, NULL);
	err_thread(ret, "pthread_create error");
	
	ret = pthread_create(&cid, NULL, consumer, NULL);
	err_thread(ret, "pthread_create error");

	ret = pthread_create(&cid, NULL, consumer, NULL);
	err_thread(ret, "pthread_create error");

	pthread_join(pid, NULL);
	pthread_join(cid, NULL);

	return 0; 
}
