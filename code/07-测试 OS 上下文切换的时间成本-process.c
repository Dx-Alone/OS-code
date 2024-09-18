#define _GNU_SOURCE  // 确保启用GNU扩展
#include <stdio.h>
#include <stdlib.h>  // for exit()
#include <unistd.h>  // for fork(), pipe(), read(), write(), wait()
#include <sched.h>  // for CPU_SET, CPU_ZERO, sched_setaffinity, sched_getcpu
#include <time.h>  // for clock_gettime() timespec CLOCK_MONOTONIC
#include <sys/types.h>  // for pid_t
#include <sys/wait.h>  // for wait()

#define NUM_SWITCHES 1000000

// 设置进程 CPU 亲和性（将进程绑定到指定 CPU 核心）
void set_affinity(int cpu)
{
    // 定义 CPU 结构体（位数组）每个位（1 / 0）代表该 CPU 核心是否在集合中
    cpu_set_t cpuset;
    // 初始化 CPU 结构体全为 0
    CPU_ZERO(&cpuset);
    // 将指定的需要被亲和的 CPU 核心添加到集合
    CPU_SET(cpu, &cpuset);
    // sched_setaffinity 系统调用设置进程 CPU 亲和性
    // 参数 0 表示当前进程，sizeof(cpu_set_t) 表示 CPU 集合大小，&cpuset 表示 CPU 集合
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) == -1)
    {
        perror("sched_setaffinity");
        exit(1);
    }
}

// 获的当前进程使用的 CPU 核心
int get_cpu()
{
    int cpu = sched_getcpu();
    printf("Now process is bind to CPU %d\n", cpu);
    return cpu;
}

int main()
{
    int pipe1[2], pipe2[2];
    pid_t pid;
    char buf = 'x';  // 一个字节的数据
    struct timespec start, end;

    // 创建两个管道
    // pipe() 函数创建管道并返回两个文件描述符（管道描述符）
    // pipe[0] 用于读，pipe[1] 用于写
    // 单向数据流，只能从 pipe[0] 读，从 pipe[1] 写
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1)
    {
        perror("pipe");
        exit(1);
    }

    // 创建子进程
    if ((pid = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }

    // 子进程
    if (pid == 0)
    {                  
        set_affinity(get_cpu()); // 将子进程绑定到 CPU 0
        
        for (int i = 0; i < NUM_SWITCHES; i++)
        {
            // read() write() 系统调用用于从管道读写数据
            // 参数 0 表示管道文件描述符，&buf 表示读写缓冲区，1 表示读写字节数
            // read() 在管道中没有数据的时候会阻塞
            // write() 在缓冲区满的时候会阻塞
            // 当系统调用阻塞了，OS 开始调度，CPU 发生上下文切换
            read(pipe1[0], &buf, 1);  // 从管道1读取
            write(pipe2[1], &buf, 1); // 写入管道2
        }

        exit(0);
    }

    // 父进程
    else
    {                  
        set_affinity(get_cpu()); // 将父进程绑定到 CPU 0

        clock_gettime(CLOCK_MONOTONIC, &start);

        for (int i = 0; i < NUM_SWITCHES; i++)
        {
            write(pipe1[1], &buf, 1); // 写入管道1
            read(pipe2[0], &buf, 1);  // 从管道2读取
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        wait(NULL); // 等待子进程结束

        long long elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
        double avg_switch_time = (double)elapsed_ns / (NUM_SWITCHES * 2);

        printf("Average context switch time: %.3f ns\n", avg_switch_time);
    }

    return 0;
}

// 测试上下文具体切换次数 perf
// sudo perf stat -e context-switches -e cpu-migrations -e page-faults binary/07
// context-switches: 操作系统上下文切换次数
// cpu-migrations: 进程从一个 CPU 核心迁移到另一个 CPU 核心的总次数
// page-faults: 进程发生缺页异常的次数