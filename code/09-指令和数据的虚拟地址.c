#include <stdio.h>
#include <stdlib.h>

// 全局变量
// .data 和 .bss 段紧密排列
int global_interger = 100;  // .data 段
int global_vector[10];  // .bss 段

int main() {
    // 程序段的地址
    printf("Location of main: %p\n", (void*)main);
    // 全局变量的地址
    printf("Location of global_interger: %p\n", (void*)&global_interger);
    printf("Location of global_vector: %p\n", (void*)&global_vector);
    // 局部变量的地址
    int local_interger = 200;
    printf("Location of local_interger: %p\n", (void*)&local_interger);
    // 申请的堆区内存地址
    int *p = (void*)malloc(1);
    printf("Location of heap memory: %p\n", p);
    // 静态变量的地址
    // 已初始化的静态变量位于 .data 段
    // 未初始化的静态变量位于 .bss 段
    static int static_interger_1 = 300;
    static int static_interger_2;
    printf("Location of static_interger_1: %p\n", (void*)&static_interger_1);
    printf("Location of static_interger_2: %p\n", (void*)&static_interger_2);
}
