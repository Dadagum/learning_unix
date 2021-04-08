#include <signal.h>
#include "curr_time.h" /* Declaration of currTime() */
#include "tlpi_hdr.h"
#define SYNC_SIG SIGUSR1 /* Synchronization signal */

// 本程序为使用 signal 同步两个进程的例子

static void              /* Signal handler - does nothing but return */
handler(int sig)
{
}

int main(int argc, char *argv[])
{
    pid_t childPid;
    sigset_t blockMask, origMask, emptyMask;
    struct sigaction sa;
    setbuf(stdout, NULL); /* Disable buffering of stdout */

    // 阻塞 SIG_BLOCK 信号
    sigemptyset(&blockMask);
    sigaddset(&blockMask, SYNC_SIG); /* Block signal */
    if (sigprocmask(SIG_BLOCK, &blockMask, &origMask) == -1)
        errExit("sigprocmask");
    sigemptyset(&sa.sa_mask);

    // 设置 SYNC_SIG 的 signal handler
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if (sigaction(SYNC_SIG, &sa, NULL) == -1)
        errExit("sigaction");

    switch (childPid = fork())
    {
    case -1:
        errExit("fork");
    case 0: /* Child */
        /* Child does some required action here... */
        printf("[%s %ld] Child started - doing some work\n",
               currTime("%T"), (long)getpid());
        sleep(2); /* Simulate time spent doing some work */
        /* And then signals parent that it's done */
        printf("[%s %ld] Child about to signal parent\n",
               currTime("%T"), (long)getpid());
        
        // 给父进程发送 SYNC_SIG 信号
        if (kill(getppid(), SYNC_SIG) == -1)
            errExit("kill");
        /* Now child can do other things... */
        _exit(EXIT_SUCCESS);
    default: /* Parent */
        /* Parent may do some work here, and then waits for child to
        complete the required action */
        printf("[%s %ld] Parent about to wait for signal\n",
               currTime("%T"), (long)getpid());
        sigemptyset(&emptyMask);
        // 暂时将 mask 置为 emptyMask，然后 suspend 进程，直到有 signal 来了，然后从 signal handler 从返回，signal mask 恢复到 sigsuspend 之前的状态
        if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
            errExit("sigsuspend");
        printf("[%s %ld] Parent got signal\n", currTime("%T"), (long)getpid());
        /* If required, return signal mask to its original state */
        if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1)
            errExit("sigprocmask");
        /* Parent carries on to do other things... */
        exit(EXIT_SUCCESS);
    }
}