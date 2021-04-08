#include <fcntl.h>
#include "tlpi_hdr.h"

// 在 exec() 一个新程序后，带有 close-on-exec flag 的文件就会自动关闭（关闭对新进程无用的文件） 

int main(int argc, char *argv[])
{
    int flags;
    if (argc > 1)
    {
        // 使用 fcntl 函数来设置 close-on-exec flag
        flags = fcntl(STDOUT_FILENO, F_GETFD); 
        if (flags == -1)
            errExit("fcntl - F_GETFD");
        flags |= FD_CLOEXEC;                            
        if (fcntl(STDOUT_FILENO, F_SETFD, flags) == -1)
            errExit("fcntl - F_SETFD");
    }
    // 执行新程序(记得最后需要加入空指针)
    execlp("ls", "ls", "-l", argv[0], (char *)NULL);
    errExit("execlp"); // 不会执行到这里
}