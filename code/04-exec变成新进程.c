#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    printf("Father begins\n");
    pid_t c_pid = fork();
    if (c_pid < 0)
    {
        fprintf(stderr, "Fork failed\n");
    }
    else if (c_pid == 0)
    {
        printf("Child begins\n");
        int temp_var = 0;
        // 子进程中执行 exec() 会直接将子进程替换为新程序
        // pid 不变，文件描述符保留
        // 若 exec() 成功，原子进程的堆、栈和数据段等都会被新程序的相应部分覆盖
        // 若 exec() 失败，则不会被覆盖
        // execl / execv / execvp / execle / execve / execvpe
        // 带 v 第二个参数是指向命令行参数数组的指针
        // 带 e 可以携带新的环境变量
        char *args[] = {"ls", "-l", "-a", NULL};
        execvp("lm", args);
        // exec() 执行成功不返回，后续代码不执行；执行失败返回 -1,继续执行后续代码
        // 无论成功失败不影响父进程
        printf("<ls %s, %s> failed\n", "-l", "-a");
        printf("If failed, temp var still works: %d\n", temp_var);
        exit(1);
    }
    else
    {
        waitpid(c_pid, NULL, 0);
        printf("Father continues\n");
    }
}