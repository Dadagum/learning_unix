#include <pthread.h>
#include "tlpi_hdr.h"

// 线程执行函数
static void *
threadFunc(void *arg)
{
    char *s = (char *)arg;
    printf("%s", s);
    return (void *)strlen(s);
}

int main(int argc, char *argv[])
{
    pthread_t t1;
    void *res;
    int s;

    // 创建线程，第二个参数是线程的属性，不需要的话直接 NULL
    s = pthread_create(&t1, NULL, threadFunc, "Hello world\n");
    if (s != 0)
        errExitEN(s, "pthread_create");
    printf("Message from main()\n");

    // 等待子线程退出
    s = pthread_join(t1, &res);
    if (s != 0)
        errExitEN(s, "pthread_join");

    // 打印子线程计算结果
    printf("Thread returned %ld\n", (long)res);
    exit(EXIT_SUCCESS);
}