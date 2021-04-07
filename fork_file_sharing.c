#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"

// 在 fork 后，子进程拷贝了父进程的 fd，指向同一个打开文件表项，实现文件共享（write不会覆盖）

int main(int argc, char *argv[])
{
    int fd, flags;

    // 创建临时文件，以 XXXXXX 结尾，函数会使用随机的字符串替换
    char template[] = "/tmp/testXXXXXX";
    setbuf(stdout, NULL); /* Disable buffering of stdout */
    fd = mkstemp(template);
    if (fd == -1)
        errExit("mkstemp");
    
    // fork 前打印临时文件的属性
    printf("File offset before fork(): %lld\n",
           (long long)lseek(fd, 0, SEEK_CUR));
    flags = fcntl(fd, F_GETFL);
    if (flags == -1)
        errExit("fcntl - F_GETFL");
    printf("O_APPEND flag before fork() is: %s\n",
           (flags & O_APPEND) ? "on" : "off");

    
    switch (fork())
    {
    case -1:
        errExit("fork");
    case 0: 
        /* Child: change file offset and status flags */
        if (lseek(fd, 1000, SEEK_SET) == -1)
            errExit("lseek");
        flags = fcntl(fd, F_GETFL); /* Fetch current flags */
        if (flags == -1)
            errExit("fcntl - F_GETFL");
        flags |= O_APPEND; /* Turn O_APPEND on */
        if (fcntl(fd, F_SETFL, flags) == -1)
            errExit("fcntl - F_SETFL");
        _exit(EXIT_SUCCESS);
    default: 
        /* Parent: can see file changes made by child */
        if (wait(NULL) == -1)
            errExit("wait"); /* Wait for child exit */
        printf("Child has exited\n");
        printf("File offset in parent: %lld\n",
               (long long)lseek(fd, 0, SEEK_CUR));
        flags = fcntl(fd, F_GETFL);
        if (flags == -1)
            errExit("fcntl - F_GETFL");
        printf("O_APPEND flag in parent is: %s\n",
               (flags & O_APPEND) ? "on" : "off");
        exit(EXIT_SUCCESS);
    }
}