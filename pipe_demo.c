#include <unistd.h>
#include "tlpi_hdr.h"

// 使用 pipe 系统调用进行父子进程之间的简单示例(只说明 pipe() 用法)
// 在 fork 后，父子进程分别关闭读端和写端

int main()
{
    int filedes[2];
    // 创建管道
    if (pipe(filedes) == -1) /* Create the pipe */
        errExit("pipe");
    switch (fork())
    { /* Create a child process */
    case -1:
        errExit("fork");
    case 0:                          /* Child */
        if (close(filedes[1]) == -1) /* Close unused write end */
            errExit("close");
        /* Child now reads from pipe */
        break;
    default:                         /* Parent */
        if (close(filedes[0]) == -1) /* Close unused read end */
            errExit("close");
        /* Parent now writes to pipe */
        break;
    }
    return 0;
}