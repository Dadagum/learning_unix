#define _GNU_SOURCE /* Get '_sys_nerr' and '_sys_errlist' \
 declarations from <stdio.h> */
#include <stdio.h>
#include <string.h> /* Get declaration of strerror() */
#include <pthread.h>
#include "tlpi_hdr.h"

// 多线程下保证某一个函数只会执行一次，例如 create 一个 pthread_key_t
// 尽管理论上可以在创建子线程前首先创建这个 key，但是对于 library 来说是不现实的
static pthread_once_t once = PTHREAD_ONCE_INIT;
// TSD: Thread-Specific Data，对于一个 key，每一个线程都可以存储自己的 value
static pthread_key_t strerrorKey;

#define MAX_ERROR_LEN 256 /* Maximum length of string in per-thread \
 buffer returned by strerror() */

/* Free thread-specific data buffer */
static void   
destructor(void *buf)
{
    free(buf);
}

/* One-time key creation function */
static void 
createKey(void)
{
    int s;
    /* Allocate a unique thread-specific data key and save the address
    of the destructor for thread-specific data buffers */
    s = pthread_key_create(&strerrorKey, destructor);
    if (s != 0)
        errExitEN(s, "pthread_key_create");
}

// 线程安全的 strerror
char *
strerror(int err)
{
    int s;
    char *buf;

    /* Make first caller allocate key for thread-specific data */
    // 第一个线程首先创建 TSD 即 strerrorKey
    s = pthread_once(&once, createKey);
    if (s != 0)
        errExitEN(s, "pthread_once");
    
    // 根据 key 获取当前线程的 value
    buf = pthread_getspecific(strerrorKey);
    /* If first call from this thread, allocate buffer for thread, and save its location */
    if (buf == NULL)
    { 
        // 线程没有初始化 buf，下面就是为 buf 分配内存，然后和 key 绑定
        buf = malloc(MAX_ERROR_LEN);
        if (buf == NULL)
            errExit("malloc");
        s = pthread_setspecific(strerrorKey, buf);
        if (s != 0)
            errExitEN(s, "pthread_setspecific");
    }
    // buf 已经初始化了，下面只需要打印出错误的信息
    if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL)
    {
        snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
    }
    else
    {
        strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
        buf[MAX_ERROR_LEN - 1] = '\0'; /* Ensure null termination */
    }

    return buf;
}