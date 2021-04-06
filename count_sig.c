#define _GNU_SOURCE
#include <signal.h>
// #include "signal_functions.h" /* Declaration of printSigset() */
#include "tlpi_hdr.h"

static int sigCnt[NSIG]; /* Counts deliveries of each signal */
static volatile sig_atomic_t gotSigint = 0; /* Set nonzero if SIGINT is delivered */

static void
handler(int sig)
{
    if (sig == SIGINT)
        gotSigint = 1;
    else sigCnt[sig]++; // 由于信号不是队列，因此采用数组计数的方式来记录接收到信号的次数
}
int main(int argc, char *argv[])
{
    int n, numSecs;
    sigset_t pendingMask, blockingMask, emptyMask;
    printf("%s: PID is %ld\n", argv[0], (long)getpid());

    // 未所有的 signal 都设置 handler
    for (n = 1; n < NSIG; n++)   
        (void) signal(n, handler); /* Ignore errors */
    /* If a sleep time was specified, temporarily block all signals,
    sleep (while another process sends us signals), and then
    display the mask of pending signals and unblock all signals */

    if (argc > 1)
    {
        numSecs = getInt(argv[1], GN_GT_0, NULL);
        // 设置所有的 signal
        sigfillset(&blockingMask);
        // 阻塞所有的 signals
        if (sigprocmask(SIG_SETMASK, &blockingMask, NULL) == -1)
            errExit("sigprocmask");
        // 进入睡眠
        printf("%s: sleeping for %d seconds\n", argv[0], numSecs);
        sleep(numSecs);
        // 获得此时正在 pending 的 mask
        if (sigpending(&pendingMask) == -1)
            errExit("sigpending");
        printf("%s: pending signals are: \n", argv[0]);

        //printSigset(stdout, "\t\t", &pendingMask);

        // unblock 所有的 signal
        sigemptyset(&emptyMask); /* Unblock all signals */
        if (sigprocmask(SIG_SETMASK, &emptyMask, NULL) == -1)
            errExit("sigprocmask");
    }
    while (!gotSigint) /* Loop until SIGINT caught */
        continue;
    
    // 打印所有收到过的 signal
    for (n = 1; n < NSIG; n++)
        if (sigCnt[n] != 0)
            printf("%s: signal %d caught %d time%s\n", argv[0], n,
                   sigCnt[n], (sigCnt[n] == 1) ? "" : "s");
    exit(EXIT_SUCCESS);
}