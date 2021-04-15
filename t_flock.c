#include <sys/file.h>
#include <fcntl.h>
#include "curr_time.h" /* Declaration of currTime() */
#include "tlpi_hdr.h"

// 示例程序：创建 file lock
// usage: ./f_lock filename [s/e][n]

int main(int argc, char *argv[])
{
    int fd, lock;
    const char *lname;
    if (argc < 3 || strcmp(argv[1], "--help") == 0 ||
        strchr("sx", argv[2][0]) == NULL)
        usageErr("%s file lock [sleep-time]\n"
                 " 'lock' is 's' (shared) or 'x' (exclusive)\n"
                 " optionally followed by 'n' (nonblocking)\n"
                 " 'secs' specifies time to hold lock\n",
                 argv[0]);
    // 决定 file lock 的属性
    // 读锁或者是写锁
    lock = (argv[2][0] == 's') ? LOCK_SH : LOCK_EX;
    // 非阻塞模式
    if (argv[2][1] == 'n')
        lock |= LOCK_NB;
    // 选择上锁的文件
    fd = open(argv[1], O_RDONLY); /* Open file to be locked */
    if (fd == -1)
        errExit("open");
    lname = (lock & LOCK_SH) ? "LOCK_SH" : "LOCK_EX";
    printf("PID %ld: requesting %s at %s\n", (long)getpid(), lname,
           currTime("%T"));
    // 上锁
    if (flock(fd, lock) == -1)
    {
        // 设置了非阻塞的模式，如果上锁不能成功立刻返回，erron 置为 EWOULDBLOCK
        if (errno == EWOULDBLOCK)
            fatal("PID %ld: already locked - bye!", (long)getpid());
        else
            errExit("flock (PID=%ld)", (long)getpid());
    }
    printf("PID %ld: granted %s at %s\n", (long)getpid(), lname,
           currTime("%T"));
    sleep((argc > 3) ? getInt(argv[3], GN_NONNEG, "sleep-time") : 10);
    printf("PID %ld: releasing %s at %s\n", (long)getpid(), lname,
           currTime("%T"));
    // 释放锁
    if (flock(fd, LOCK_UN) == -1)
        errExit("flock");
    exit(EXIT_SUCCESS);
}