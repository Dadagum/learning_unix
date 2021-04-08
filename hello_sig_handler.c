#include <signal.h>
#include "tlpi_hdr.h"

static void
sigHandler(int sig)
{
    // 实际上由于 std buf 是 static 的，在 signal hander 中使用不安全（不可重入）
    printf("Ouch!\n");
}

int main(int argc, char *argv[])
{
    int j;
    void (*oldHandler)(int);

    // disposition
    oldHandler = signal(SIGINT, sigHandler);
    if (oldHandler == SIG_ERR)
        errExit("signal");

    for (j = 0;; j++)
    {
        printf("%d\n", j);
        sleep(3); /* Loop slowly... */
    }
}