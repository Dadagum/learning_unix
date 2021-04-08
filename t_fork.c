#include "tlpi_hdr.h"

// 说明通过 fork，子进程“拷贝”了父进程的内存（栈、堆内存，还有 data segment ...）

static int idata = 111; /* Allocated in data segment */

int main(int argc, char *argv[])
{
    int istack = 222; /* Allocated in stack segment */
    int* isHeap = (int*) malloc(sizeof(int));
    *isHeap = 444; /* Allocated in heap segment */
    pid_t childPid;
    switch (childPid = fork())
    {
        case -1:
            errExit("fork");
        
        // child
        case 0:
            idata *= 3;
            istack *= 3;
            *isHeap = (*isHeap) * 3;
            break;

        // parent
        default:
            sleep(3); /* Give child a chance to execute */
            break;
    }
    /* Both parent and child come here */
    printf("PID=%ld %s idata=%d istack=%d isheap=%d \n ", (long)getpid(),
           (childPid == 0) ? "(child) " : "(parent)", idata, istack, *isHeap);
    exit(EXIT_SUCCESS);
}