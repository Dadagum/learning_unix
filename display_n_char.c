#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "tlpi_hdr.h"

// 打印文件第 i 个字符
// exp: ./display /dir/filename 4
int main(int argc, char* argv[]) {
    struct stat st;
    size_t len;
    int fd;
    char *buf = (char*)malloc(2*sizeof(char));
    // 参数检验和获取
    if (argc != 3)
        usageErr("argc should be 3.");
    int ith = atoi(argv[2]);
    
    // 打开文件
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) 
        errExit("open file fails.");

    // 移动文件指针
    stat(argv[1], &st);
    len = st.st_size;
    if (ith <= 0 || ith > len) 
        errExit("read fails: out of range");
    lseek(fd, ith-1, SEEK_SET);

    // 读取字符并输出
    read(fd, buf, 1);
    buf[1] = '\0';
    printf("read result: %s \n", buf);
    return 0;
}