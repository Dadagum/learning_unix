#define _GNU_SOURCE /* Obtain O_DIRECT definition from <fcntl.h> */
#include <fcntl.h>
#include <malloc.h>
#include "tlpi_hdr.h"

// usage: ./direct_read filename length [offset] [alignment]

int main(int argc, char *argv[])
{
    int fd;
    ssize_t numRead;
    size_t length, alignment;
    off_t offset;
    void *buf;

    // 获取参数
    if (argc < 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file length [offset [alignment]]\n", argv[0]);
    length = getLong(argv[2], GN_ANY_BASE, "length");
    offset = (argc > 3) ? getLong(argv[3], GN_ANY_BASE, "offset") : 0;
    alignment = (argc > 4) ? getLong(argv[4], GN_ANY_BASE, "alignment") : 4096;

    // 采用直接 io 方式打开文件
    fd = open(argv[1], O_RDONLY | O_DIRECT);
    if (fd == -1)
        errExit("open");

    // 分配 user space buffer
    buf = (char *)memalign(alignment, length);
    if (buf == NULL)
        errExit("memalign");

    // 移动文件指针到所需偏移位置    
    if (lseek(fd, offset, SEEK_SET) == -1)
        errExit("lseek");

    // 读文件并打印读出的字节
    numRead = read(fd, buf, length);
    if (numRead == -1)
        errExit("read");
    printf("Read %ld bytes\n", (long)numRead);

    free(buf);
    close(fd);
    exit(EXIT_SUCCESS);
}