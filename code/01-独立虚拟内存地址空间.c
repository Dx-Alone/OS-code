#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    int *p = malloc(sizeof(int));
    assert(p != NULL);
    // p 存储的是虚拟内存地址，并不直接指向物理内存
    // 两个独立程序的 p 地址是一样的（禁用 ASLR）
    printf("(%d) Memory Address of p: %x\n", getpid(), p);
    *p = 0;
    while (1)
    {
        sleep(1);
        *p = *p + 1;
        printf("(%d) %d\n", getpid(), *p);
    }
    return 0;
}