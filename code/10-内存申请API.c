#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
	// 通过 malloc 申请堆区内存
	int *pint = (int *)malloc(sizeof(int));
	char *pstring = (char *)malloc(strlen("hello, world") + 1);
	int *pvec1 = (int *)malloc(sizeof(int) * 10);
	printf("Sizeof pvec1: %d\n", sizeof(pvec1));  // 指针的大小：8
	int pvec2[10];
	printf("Sizeof pvec2: %d\n", sizeof(pvec2));  // 数组的大小：40（4 * 10）
	// 释放 heap 内存
	free(pint);
	free(pstring);
	free(pvec1);
	// free(pvec2); 不应该释放非堆区内存
	return 0;
}
