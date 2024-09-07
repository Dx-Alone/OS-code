#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    printf("My pid is: %d\n", (int)getpid());
    // 使用 fork() 创建子进程
    pid_t child_pid = fork();
    // 子进程从 fork() 调用结束的地方开始执行
    if (child_pid < 0)
    {
        // 如果 fork() 返回值 < 0，说明 fork() 失败
        fprintf(stderr, "[!] Fork failed\n");
        exit(1);
    }
    else if (child_pid == 0)
    {
        // fork() 返回 0,说明当前是子进程
        printf("[S] Subprocess pid is: %d, its father pid is: %d\n", (int)getpid(), (int)getppid());
    }
    else
    {
        // 父进程的 fork() 返回子进程的 pid
        printf("[F] Father process pid is: %d, its child pid is: %d\n", (int)getpid(), (int)child_pid);
        // 等待子进程
        // 这里如果不 wait，可能父进程提前结束，那么子进程会被 systemd 或 init 领养
        pid_t wc = wait(NULL);
        if (wc == -1)
        {
            perror("Wait failed");
            exit(EXIT_FAILURE);
        }
        printf("Wait Code: %d", (int)wc);
    }
    return 0;
}