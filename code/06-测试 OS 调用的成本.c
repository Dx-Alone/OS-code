#include <stdio.h>
#include <unistd.h>
#include <time.h> // clock_gettime 和 CLOCK_MONOTONIC 的头文件 连接 glibc 库

int main()
{
    // 创建表示时间的结构体变量
    struct timespec start, end;
    // 纳秒时间差距
    long long ns_diff;

    // 获取开始时间
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
    {
        perror("clock_gettime");
    }

    // 执行系统调用如 getpid()
    int i;
    int count = 10000;
    for (int i = 0; i < count; i++)
    {
        getpid();
    }

    // 获取结束时间
    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1)
    {
        perror("clock_gettime");
    }

    // 计算时间差
    ns_diff = ((end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec)) / count;
    printf("系统调用的成本是： %lld 纳秒", ns_diff);

    return 0;
}