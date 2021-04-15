#include "svshm_xfr.h"

// 实例：使用 system v 的 share memory 进行进程间的数据交换
// 由于例子中 buf 只有一个，因此只需要两个二元的读写信号量即可完成任务
// 读进程程序

int main(int argc, char *argv[])
{
    int semid, shmid, xfrs, bytes;
    struct shmseg *shmp;
    // 获取写进程创建的信号量
    semid = semget(SEM_KEY, 0, 0);
    if (semid == -1)
        errExit("semget");
    // 获取写进程创建的共享内存
    shmid = shmget(SHM_KEY, 0, 0);
    if (shmid == -1)
        errExit("shmget");
    // attach 共享内存
    shmp = shmat(shmid, NULL, SHM_RDONLY);
    if (shmp == (void *)-1)
        errExit("shmat");

    for (xfrs = 0, bytes = 0;; xfrs++)
    {
        // 获取读锁（读信号-1）
        if (reserveSem(semid, READ_SEM) == -1) /* Wait for our turn */
            errExit("reserveSem");
        if (shmp->cnt == 0) /* Writer encountered EOF */
            break;
        bytes += shmp->cnt;
        // 将共享内存中的内容在控制台输出
        if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt)
            fatal("partial/failed write");
        // 释放写锁（写信号+1）
        if (releaseSem(semid, WRITE_SEM) == -1) /* Give writer a turn */
            errExit("releaseSem");
    }
    // detach 共享内存
    if (shmdt(shmp) == -1)
        errExit("shmdt");
    // 释放写锁，让写进程最后负责清理 ipc 对象
    if (releaseSem(semid, WRITE_SEM) == -1)
        errExit("releaseSem");
    fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}