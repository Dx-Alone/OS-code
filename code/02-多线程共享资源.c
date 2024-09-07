#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

volatile int counter1 = 0;
volatile int counter2 = 0;
int loop_time;
pthread_mutex_t mutex; // 互斥锁

// 线程函数 void *
void *thread_func(void *arg)
{
    int i;
    for (i = 0; i < loop_time; i++)
    {
        counter2++;

        // 对资源 counter1 上锁
        pthread_mutex_lock(&mutex);
        counter1++;
        // 解锁
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: Missing loop time argument\n");
    }
    loop_time = atoi(argv[1]);

    // 创建两个线程
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, thread_func, NULL);
    pthread_create(&tid2, NULL, thread_func, NULL);

    // 等待两个线程结束
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    printf("Final counter1 value: %d; Final counter2 value: %d\n", counter1, counter2);

    // 销毁互斥锁
    pthread_mutex_destroy(&mutex);
    return 0;
}