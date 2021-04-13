#include <sys/types.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include "curr_time.h" /* Declaration of currTime() */
#include "semun.h"     /* Definition of semun union */
#include "tlpi_hdr.h"

// 示例程序：创建和使用 systemv 的信号量
// usage: 
// 创建和初始化信号量： ./semun init_val
// 操作信号量: ./semun semid sem_op

int main(int argc, char *argv[])
{
    int semid;
    if (argc < 2 || argc > 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s init-value\n"
                 " or: %s semid operation\n",
                 argv[0], argv[0]);
    if (argc == 2)
    { /* Create and initialize semaphore */
        union semun arg;
        // 创建信号量集合，如果创建的是新的信号量集合，则集合中只有1个信号量（第二个参数）
        semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
        if (semid == -1)
            errExit("semid");
        // 获取信号量的初始值
        arg.val = getInt(argv[1], 0, "init-value");
        // 初始化 semid 的信号量集合中的第 0 号信号量的值设置为 arg
        if (semctl(semid, /* semnum= */ 0, SETVAL, arg) == -1)
            errExit("semctl");
        printf("Semaphore ID = %d\n", semid);
    }
    else
    {   /* Perform an operation on first semaphore */
        struct sembuf sop; /* Structure defining operation */
        semid = getInt(argv[1], 0, "semid");
        sop.sem_num = 0; /* Specifies first semaphore in set */
        sop.sem_op = getInt(argv[2], 0, "operation");
        /* Add, subtract, or wait for 0 */
        sop.sem_flg = 0; /* No special options for operation */
        printf("%ld: about to semop at %s\n", (long)getpid(), currTime("%T"));
        // PV 操作
        if (semop(semid, &sop, 1) == -1)
            errExit("semop");
        printf("%ld: semop completed at %s\n", (long)getpid(), currTime("%T"));
    }
    exit(EXIT_SUCCESS);
}