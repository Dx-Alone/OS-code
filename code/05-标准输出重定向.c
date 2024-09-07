#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h> // for mode_t

int main()
{
    pid_t cpid = fork();
    if (cpid < 0)
    {
        printf("Fork failed\n");
    }
    else if (cpid == 0)
    {
        // 关闭标准输出
        close(STDOUT_FILENO);
        // 打开输入重定向到的文件
        // 创建（读写执行） + 写 + 截断
        int fd = open("output/_05.output", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
        // 原理：
        // - 在 Unix/Linux 系统中，文件描述符是小整数，通常从 0 开始分配
        // - 标准输入、标准输出、标准错误分别对应文件描述符 0、1、2
        // - 关闭文件描述符 1 (STDOUT_FILENO) 时，这个文件描述符变得可用
        // - 调用 open() 函数时，系统会分配一个最小的未使用文件描述符
        // - 由于刚刚关闭了文件描述符 1，系统将会分配文件描述符 1 给新打开的文件 output/_05.output
        // - 现在文件描述符 1 指向 output/_05.output 文件
        // - 所有写入标准输出的内容（例如 printf() 输出）将会写入 output/_05.output 文件，而不是终端或控制台
        if (fd == -1)
        {
            perror("Open falied");
        }
        // 文件描述符被保留，因此 exec() 执行的程序输出会重定向
        execlp("ls", "ls", "-l", "-a", NULL);
        // 若执行失败，子进程中文件描述符状态不变——依然重定位
        printf("Exec failed");
    }
    else
    {
        waitpid(cpid, NULL, 0);
        // 调用 cat 查看
        // 子进程的文件描述符状态不影响父进程，每个进程文件描述符是独立的
        printf("Cat 查看重定向的结果：\n");
        // 需要立刻刷新缓冲区写入终端，否则由于下面执行 execlp 会导致缓冲区数据被新的进程覆盖掉
        fflush(stdout);
        execlp("cat", "cat", "output/_05.output", NULL);
        perror("Cat failed");
    }
}
