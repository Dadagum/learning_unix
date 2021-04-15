#include <sys/mman.h>
#include <fcntl.h>
#include "tlpi_hdr.h"
#define MEM_SIZE 10

// 实例：使用 mmap 将 file 映射到内存中

int main(int argc, char *argv[])
{
    char *addr;
    int fd;
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file [new-value]\n", argv[0]);
    fd = open(argv[1], O_RDWR);
    if (fd == -1)
        errExit("open");
    // 映射文件，共享模式，可读可写
    addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
        errExit("mmap");
    if (close(fd) == -1) /* No longer need 'fd' */
        errExit("close");
    printf("Current string=%.*s\n", MEM_SIZE, addr);
    
    // 将 argv[2] 字符串写到文件中
    if (argc > 2)
    { 
        int cp_size = strlen(argv[2]);
        if (cp_size >= MEM_SIZE)
            cmdLineErr("'new-value' too large\n");
        //memset(addr, 0, MEM_SIZE); 
        // 将字符串 copy 到内存，最后留 \0
        strncpy(addr, argv[2], cp_size);
        // msync: synchronize a file with a memory map
        if (msync(addr, MEM_SIZE, MS_SYNC) == -1)
            errExit("msync");
        printf("Copied \"%s\" to shared memory\n", argv[2]);
    }
    exit(EXIT_SUCCESS);
}