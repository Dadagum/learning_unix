#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "tlpi_hdr.h"

// 示例：创建消息队列
// usage: ./msg_create [-c] [-f filename] [-k key] [-p] [-x]

static void /* Print usage info, then exit */
usageError(const char *progName, const char *msg)
{
    if (msg != NULL)
        fprintf(stderr, "%s", msg);
    fprintf(stderr, "Usage: %s [-cx] {-f pathname | -k key | -p} "
                    "[octal-perms]\n",
            progName);
    fprintf(stderr, " -c Use IPC_CREAT flag\n");
    fprintf(stderr, " -x Use IPC_EXCL flag\n");
    fprintf(stderr, " -f pathname Generate key using ftok()\n");
    fprintf(stderr, " -k key Use 'key' as key\n");
    fprintf(stderr, " -p Use IPC_PRIVATE key\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int numKeyFlags; /* Counts -f, -k, and -p options */
    int flags, msqid, opt;
    unsigned int perms;
    long lkey;
    key_t key;
    /* Parse command-line options and arguments */
    numKeyFlags = 0;
    flags = 0;

    // getopt 函数：解析命令参数，字符后带冒号表示需要带上 argument
    while ((opt = getopt(argc, argv, "cf:k:px")) != -1)
    {
        switch (opt)
        {
        case 'c':
            // 表示创建
            flags |= IPC_CREAT;
            break;
        case 'f': /* -f pathname */
            // arg 从 optarg 获取
            key = ftok(optarg, 1);
            if (key == -1)
                errExit("ftok");
            numKeyFlags++;
            break;
        case 'k': /* -k key (octal, decimal or hexadecimal) */
            // 直接使用输入的 key 值作为 key
            if (sscanf(optarg, "%li", &lkey) != 1)
                cmdLineErr("-k option requires a numeric argument\n");
            key = lkey;
            numKeyFlags++;
            break;
        case 'p':
            // 直接由 os 指定
            key = IPC_PRIVATE;
            numKeyFlags++;
            break;
        case 'x':
            // 创建的消息队列如果已经存在，则返回 EEXIST
            flags |= IPC_EXCL;
            break;
        default:
            usageError(argv[0], "Bad option\n");
        }
    }
    if (numKeyFlags != 1)
        usageError(argv[0], "Exactly one of the options -f, -k, "
                            "or -p must be supplied\n");
    perms = (optind == argc) ? (S_IRUSR | S_IWUSR) : getInt(argv[optind], GN_BASE_8, "octal-perms");
    // 创建消息队列
    msqid = msgget(key, flags | perms);
    if (msqid == -1)
        errExit("msgget");
    // 控制台打印出创建成功的 msqid
    printf("%d\n", msqid);
    exit(EXIT_SUCCESS);
}