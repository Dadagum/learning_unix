#include <pthread.h>
#include "tlpi_hdr.h"

// 在线程创建时就设置线程的 detach，尽管也可以子线程随后调用以下函数来 detach 自己
// pthread_detach(pthread_self());

// 线程执行函数
static void *
threadFunc(void *arg)
{
    char *s = (char *)arg;
    printf("%s", s);
    return (void *)strlen(s);
}

int main()
{
    pthread_t thr;
    pthread_attr_t attr;
    int s;
    s = pthread_attr_init(&attr); /* Assigns default values */
    if (s != 0)
        errExitEN(s, "pthread_attr_init");
    s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (s != 0)
        errExitEN(s, "pthread_attr_setdetachstate");
    s = pthread_create(&thr, &attr, threadFunc, (void *)1);
    if (s != 0)
        errExitEN(s, "pthread_create");
    // 在线程创建成功后，attr 可以不用了
    s = pthread_attr_destroy(&attr); /* No longer needed */
    if (s != 0)
        errExitEN(s, "pthread_attr_destroy");
}