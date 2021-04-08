#define _GNU_SOURCE
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sched.h>
#include "tlpi_hdr.h"

// 本程序使用 clone 来创建子进程

#ifndef CHILD_SIG
#define CHILD_SIG SIGUSR1 /* Signal to be generated on termination \
 of cloned child */
#endif

static int /* Startup function for cloned child */
childFunc(void *arg)
{
    if (close(*((int *)arg)) == -1)
        errExit("close");
    return 0; /* Child terminates now */
}

int main(int argc, char *argv[])
{
    const int STACK_SIZE = 65536; /* Stack size for cloned child */
    char *stack;                  /* Start of stack buffer */
    char *stackTop;               /* End of stack buffer */
    int s, fd, flags;

    fd = open("/dev/null", O_RDWR); /* Child will close this fd */
    if (fd == -1)
        errExit("open");
    /* If argc > 1, child shares file descriptor table with parent */
    // 如果是共享的 fd table（CLONE_FILES），那么子进程关闭了 fd，则父进程的 fd 就无效了；否则即使是子进程关闭了 fd，父进程仍然可以进行文件读写
    flags = (argc > 1) ? CLONE_FILES : 0;

    /* Allocate stack for child */
    stack = malloc(STACK_SIZE);
    if (stack == NULL)
        errExit("malloc");
    stackTop = stack + STACK_SIZE; /* Assume stack grows downward */

    /* Ignore CHILD_SIG, in case it is a signal whose default is to
    terminate the process; but don't ignore SIGCHLD (which is ignored
    by default), since that would prevent the creation of a zombie. */
    if (CHILD_SIG != 0 && CHILD_SIG != SIGCHLD)
        if (signal(CHILD_SIG, SIG_IGN) == SIG_ERR)
            errExit("signal");
    /* Create child; child commences execution in childFunc() */
    // 子进程会执行 childFunc，不是像 fork 那样往下继续执行
    if (clone(childFunc, stackTop, flags | CHILD_SIG, (void *)&fd) == -1)
        errExit("clone");

    /* Parent falls through to here. Wait for child; __WCLONE is
    needed for child notifying with signal other than SIGCHLD. */
    if (waitpid(-1, NULL, (CHILD_SIG != SIGCHLD) ? __WCLONE : 0) == -1)
        errExit("waitpid");
    printf("child has terminated\n");

    /* Did close() of file descriptor in child affect parent? */
    // 下面都是检查父进程的 fd 是否还是可以进行读写的
    s = write(fd, "x", 1);
    if (s == -1 && errno == EBADF)
        printf("file descriptor %d has been closed\n", fd);
    else if (s == -1)
        printf("write() on file descriptor %d failed "
               "unexpectedly (%s)\n",
               fd, strerror(errno));
    else
        printf("write() on file descriptor %d succeeded\n", fd);
    exit(EXIT_SUCCESS);
}