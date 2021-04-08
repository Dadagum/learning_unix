#include <signal.h>
#include "tlpi_hdr.h"

// 示例：block 某一个信号，进行某一些处理，然后重新 unblock

int main() {
    sigset_t blockSet, prevMask;
    // 初始化需要阻塞的信号组
    sigemptyset(&blockSet);
    // 往信号组加入某个信号（例如 SIGINT）
    sigaddset(&blockSet, SIGINT);
    // 阻塞，返回以前的 signal mask
    if (sigprocmask(SIG_BLOCK, &blockSet, &prevMask) == -1)
        errExit("sigprocmask1");
    /* ... Code that should not be interrupted by SIGINT ... */

    /* Restore previous signal mask, unblocking SIGINT */
    if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
        errExit("sigprocmask2");
    return 0;
}