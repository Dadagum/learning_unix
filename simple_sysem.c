#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

// 实现简单的 system 函数，就是 fork 子进程，子进程再 execl 一个进程执行 sh -c comd 这个命令
// 书本还有一种改进过的实现方法

int system(char *command)
{
    int status;
    pid_t childPid;
    switch (childPid = fork())
    {
    case -1: /* Error */
        return -1;
    case 0: /* Child */
        execl("/bin/sh", "sh", "-c", command, (char *)NULL);
        _exit(127); /* Failed exec */
    default: /* Parent */
        if (waitpid(childPid, &status, 0) == -1)
            return -1;
        else
            return status;
    }
}