#define _XOPEN_SOURCE 600
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "tlpi_hdr.h"

// 本程序说明 signal handler 只能够调用可重入函数
// wsl 报错:  ../sysdeps/x86_64/multiarch/strlen-avx2.S: No such file or directory.
// usage: a.out str1 str2

static char *str2;      /* Set from argv[2] */
static int handled = 0; /* Counts number of calls to handler */

static void
handler(int sig)
{
    crypt(str2, "xx");
    handled++;
}

int main(int argc, char *argv[])
{
    char *cr1;
    int callNum, mismatch;
    struct sigaction sa;

    // 参数检验与提取
    if (argc != 3)
        usageErr("%s str1 str2\n", argv[0]);
    str2 = argv[2];                     /* Make argv[2] available to handler */
    // strdup 内部适用 malloc 分配内存的，最后需要 free
    cr1 = strdup(crypt(argv[1], "xx")); /* Copy statically allocated string
 to another buffer */
    if (cr1 == NULL)
        errExit("strdup");

    // 设置 handler
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(SIGINT, &sa, NULL) == -1)
        errExit("sigaction");

    /* Repeatedly call crypt() using argv[1]. If interrupted by a
    signal handler, then the static storage returned by crypt()
    will be overwritten by the results of encrypting argv[2], and
    strcmp() will detect a mismatch with the value in 'cr1'. */
    for (callNum = 1, mismatch = 0;; callNum++)
    {
        if (strcmp(crypt(argv[1], "xx"), cr1) != 0)
        {
            mismatch++;
            printf("Mismatch on call %d (mismatch=%d handled=%d)\n",
                   callNum, mismatch, handled);
        }
    }
    return 0;
}