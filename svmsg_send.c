#include <sys/types.h>
#include <sys/msg.h>
#include "tlpi_hdr.h"
#define MAX_MTEXT 1024

// 示例程序：往某个消息队列发送消息
// usage: ./msg_send [-n] msqid mqtype [mtext]

struct mbuf
{
    long mtype;            /* Message type */
    char mtext[MAX_MTEXT]; /* Message body */
};

static void /* Print (optional) message, then usage description */
usageError(const char *progName, const char *msg)
{
    if (msg != NULL)
        fprintf(stderr, "%s", msg);
    fprintf(stderr, "Usage: %s [-n] msqid msg-type [msg-text]\n", progName);
    fprintf(stderr, " -n Use IPC_NOWAIT flag\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int msqid, flags, msgLen;
    struct mbuf msg; /* Message buffer for msgsnd() */
    int opt;         /* Option character from getopt() */
    /* Parse command-line options and arguments */
    flags = 0;
    while ((opt = getopt(argc, argv, "n")) != -1)
    {
        // 设置 non-blocking，如果设置了消息队列满了，则会立刻返回，errno 置为 EAGAIN
        if (opt == 'n')
            flags |= IPC_NOWAIT;
        else
            usageError(argv[0], NULL);
    }
    if (argc < optind + 2 || argc > optind + 3)
        usageError(argv[0], "Wrong number of arguments\n");
    msqid = getInt(argv[optind], 0, "msqid");
    msg.mtype = getInt(argv[optind + 1], 0, "msg-type");
    if (argc > optind + 2)
    { /* 'msg-text' was supplied */
        msgLen = strlen(argv[optind + 2]) + 1;
        if (msgLen > MAX_MTEXT)
            cmdLineErr("msg-text too long (max: %d characters)\n", MAX_MTEXT);
        memcpy(msg.mtext, argv[optind + 2], msgLen);
    }
    else
    { /* No 'msg-text' ==> zero-length msg */
        msgLen = 0;
    }
    /* Send message */
    if (msgsnd(msqid, &msg, msgLen, flags) == -1)
        errExit("msgsnd");
    exit(EXIT_SUCCESS);
}