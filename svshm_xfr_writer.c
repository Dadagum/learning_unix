#include "semun.h" /* Definition of semun union */
#include "svshm_xfr.h"

// 实例：使用 system v 的 share memory 进行进程间的数据交换
// 由于例子中 buf 只有一个，因此只需要两个二元的读写信号量即可完成任务
// 写进程程序

int main(int argc, char *argv[])
{
    int semid, shmid, bytes, xfrs;
    struct shmseg *shmp;
    union semun dummy;
    // 获取信号量
    semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
    if (semid == -1)
        errExit("semget");
    // 将写信号量置为1，读信号置为0，因为只有先写才能读
    if (initSemAvailable(semid, WRITE_SEM) == -1)
        errExit("initSemAvailable");
    if (initSemInUse(semid, READ_SEM) == -1)
        errExit("initSemInUse");
    
    // 创建/得到共享内存
    shmid = shmget(SHM_KEY, sizeof(struct shmseg), IPC_CREAT | OBJ_PERMS);
    if (shmid == -1)
        errExit("shmget");
    // attach 共享内存
    shmp = shmat(shmid, NULL, 0);
    if (shmp == (void *)-1)
        errExit("shmat");

    // 从控制台中不断读入数据
    for (xfrs = 0, bytes = 0;; xfrs++, bytes += shmp->cnt)
    {
        // 获取写锁（准确讲是将写信号量-1）
        if (reserveSem(semid, WRITE_SEM) == -1) /* Wait for our turn */
            errExit("reserveSem");
        // 读取输入数据到共享内存中
        shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
        if (shmp->cnt == -1)
            errExit("read");
        // 释放读锁（讲读信号量+1），让读者可以读取共享内存中的数据
        if (releaseSem(semid, READ_SEM) == -1) /* Give reader a turn */
            errExit("releaseSem");
        /* Have we reached EOF? We test this after giving the reader
 a turn so that it can see the 0 value in shmp->cnt. */
        if (shmp->cnt == 0)
            break;
    }
    // 再次获取写锁（将写信号量-1），保证读者读完数据才进行 ipc 对象的删除
    if (reserveSem(semid, WRITE_SEM) == -1)
        errExit("reserveSem");
    // 删除信号量
    if (semctl(semid, 0, IPC_RMID, dummy) == -1)
        errExit("semctl");
    // detach 共享内存
    if (shmdt(shmp) == -1)
        errExit("shmdt");
    // 删除共享内存
    if (shmctl(shmid, IPC_RMID, 0) == -1)
        errExit("shmctl");
    fprintf(stderr, "Sent %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}