#include <sys/wait.h>
#include "tlpi_hdr.h"

// 采用管道，完成 ls | wc -l
// 大概思路：
// 父进程依次 fork 两个子进程，分别会执行 ls 和 wc 命令
// 负责 ls 命令的子进程可以关闭读端，然后将 stdout 重定位到输出 fd 中
// 负责 wc 命令的子进程可以关闭写端，然后将 stdin 重定位到输入的 fd 中
// 最后父进程再关闭自己的两个 fd，wait 两个子进程 

int main(int argc, char *argv[])
{
    int pfd[2];          /* Pipe file descriptors */
    if (pipe(pfd) == -1) /* Create pipe */
        errExit("pipe");

    switch (fork())
    {
        case -1:
            errExit("fork");
        case 0:  /* First child: exec 'ls' to write to pipe */
            // 关闭写端
            if (close(pfd[0]) == -1)
                errExit("close 1");
            // 标准写法：将 stdout 重定位到 pdf[1] 即输出端
            if (pfd[1] != STDOUT_FILENO)
            { /* Defensive check */
                if (dup2(pfd[1], STDOUT_FILENO) == -1)
                    errExit("dup2 1");
                if (close(pfd[1]) == -1)
                    errExit("close 2");
            }
            // 执行 ls 命令
            execlp("ls", "ls", (char *)NULL); /* Writes to pipe */
            errExit("execlp ls");
        default: /* Parent falls through to create next child */
            break;
    }
    switch (fork())
    {
        case -1:
            errExit("fork");
        case 0:  /* Second child: exec 'wc' to read from pipe */
            // 关闭写端
            if (close(pfd[1]) == -1) 
                errExit("close 3");
            // 将 stdin 重定位到 pdf[0]
            if (pfd[0] != STDIN_FILENO)
            { /* Defensive check */
                if (dup2(pfd[0], STDIN_FILENO) == -1)
                    errExit("dup2 2");
                if (close(pfd[0]) == -1)
                    errExit("close 4");
            }
            // 执行 wc 命令，此时就会从 stdin(定位后就是输入端) 读取 ls 的输出，然后将 wc 结果输出到 stdout
            execlp("wc", "wc", "-l", (char *)NULL); /* Reads from pipe */
            errExit("execlp wc");
        default: /* Parent falls through */
            break;
    }
    /* Parent closes unused file descriptors for pipe, and waits for children */
    if (close(pfd[0]) == -1)
        errExit("close 5");
    if (close(pfd[1]) == -1)
        errExit("close 6");
    if (wait(NULL) == -1)
        errExit("wait 1");
    if (wait(NULL) == -1)
        errExit("wait 2");
    exit(EXIT_SUCCESS);
}