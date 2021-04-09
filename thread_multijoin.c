#include <pthread.h>
#include "tlpi_hdr.h"


static pthread_cond_t threadDied = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t threadMutex = PTHREAD_MUTEX_INITIALIZER;

/* Protects all of the following global variables */
static int totThreads = 0;  /* Total number of threads created */
static int numLive = 0;     /* Total number of threads still alive or
 terminated but not yet joined */
static int numUnjoined = 0; /* Number of terminated threads that
 have not yet been joined */

enum tstate
{                /* Thread states */
  TS_ALIVE,      /* Thread is alive */
  TS_TERMINATED, /* Thread terminated, not yet joined */
  TS_JOINED      /* Thread terminated, and joined */
};

// 记录 thread 
static struct
{                      /* Info about each thread */
    pthread_t tid;     /* ID of this thread */
    enum tstate state; /* Thread state (TS_* constants above) */
    int sleepTime;     /* Number seconds to live before terminating */
} * thread;

static void * /* Start function for thread */
threadFunc(void *arg)
{
    int idx = *((int *)arg);
    int s;
    sleep(thread[idx].sleepTime); /* Simulate doing some work */
    printf("Thread %d terminating\n", idx);

    // 保护临界区：更新 numUnjoined 变量
    s = pthread_mutex_lock(&threadMutex);
    if (s != 0)
        errExitEN(s, "pthread_mutex_lock");
    numUnjoined++;
    thread[idx].state = TS_TERMINATED;
    s = pthread_mutex_unlock(&threadMutex);

    if (s != 0)
        errExitEN(s, "pthread_mutex_unlock");
    // 条件变量：通知主线程线程已经结束
    s = pthread_cond_signal(&threadDied);
    if (s != 0)
        errExitEN(s, "pthread_cond_signal");
    return NULL;
}

int main(int argc, char *argv[])
{
    int s, idx;
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s nsecs...\n", argv[0]);

    // 线程数组  
    totThreads = argc - 1;
    thread = calloc(totThreads, sizeof(*thread));
    if (thread == NULL)
        errExit("calloc");
    /* Create all threads */
    for (idx = 0; idx < argc - 1; idx++)
    {
        thread[idx].sleepTime = getInt(argv[idx + 1], GN_NONNEG, NULL);
        thread[idx].state = TS_ALIVE;
        // 
        s = pthread_create(&thread[idx].tid, NULL, threadFunc, &idx);
        if (s != 0)
            errExitEN(s, "pthread_create");
    }
    // “正在运行”线程总数
    numLive = totThreads;

    /* Join with terminated threads */
    while (numLive > 0)
    {
        // 后面要结合条件变量判断 numUnjoined 的值，因此需要先上锁
        s = pthread_mutex_lock(&threadMutex);
        if (s != 0)
            errExitEN(s, "pthread_mutex_lock");
        // 采用条件变量
        while (numUnjoined == 0)
        {
            // 还没有线程结束，先释放锁，等待线程唤醒
            s = pthread_cond_wait(&threadDied, &threadMutex);
            if (s != 0)
                errExitEN(s, "pthread_cond_wait");
        }

        // 来到这里说明 numUnjoined > 0 ，下面就是循环遍历线程的状态，看哪一个是 TS_TERMINATED
        for (idx = 0; idx < totThreads; idx++)
        {
            if (thread[idx].state == TS_TERMINATED)
            {
                s = pthread_join(thread[idx].tid, NULL);
                if (s != 0)
                    errExitEN(s, "pthread_join");
                // 更新变量
                thread[idx].state = TS_JOINED;
                numLive--;
                numUnjoined--;
                printf("Reaped thread %d (numLive=%d)\n", idx, numLive);
            }
        }
        s = pthread_mutex_unlock(&threadMutex);

        if (s != 0)
            errExitEN(s, "pthread_mutex_unlock");
    }
    exit(EXIT_SUCCESS);
}