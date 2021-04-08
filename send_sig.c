#include <signal.h>
#include "get_num.h"
#include "tlpi_hdr.h"

// usage: ./send_sig pid sig-num
int main(int argc, char *argv[])
{
    // s: 需要将信号发送给哪一个进程
    int s, sig;
    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s sig-num pid\n", argv[0]);
    sig = getInt(argv[2], 0, "sig-num");
    s = kill(getLong(argv[1], 0, "pid"), sig); 

    if (sig != 0)
    {
        if (s == -1)
            errExit("kill");
    }
    else
    { 
        /* Null signal: process existence check */
        if (s == 0)
        {
            printf("Process exists and we can send it a signal\n");
        }
        else
        {
            if (errno == EPERM)
                printf("Process exists, but we don't have "
                       "permission to send it a signal\n");
            else if (errno == ESRCH)
                printf("Process does not exist\n");
            else
                errExit("kill");
        }
    }
    exit(EXIT_SUCCESS);
}