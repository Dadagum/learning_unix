#include <stdio.h>
#include <string.h> /* Get declaration of strerror() */
#include <pthread.h>
#include "tlpi_hdr.h"

// 测试 strerro 函数是否线程安全

// 线程执行函数
static void *
threadFunc(void *arg)
{
    char *str;
    printf("Other thread about to call strerror()\n");
    // 测试函数
    str = strerror(EPERM);
    printf("Other thread: str (%p) = %s\n", str, str);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t t;
    int s;
    char *str;
    // 主线程调用
    str = strerror(EINVAL);
    printf("Main thread has called strerror()\n");
    // 创建子线程调用
    s = pthread_create(&t, NULL, threadFunc, NULL);
    if (s != 0)
        errExitEN(s, "pthread_create");
    // 等待子线程结束
    s = pthread_join(t, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    printf("Main thread: str (%p) = %s\n", str, str);
    exit(EXIT_SUCCESS);
}