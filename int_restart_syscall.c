#include "tlpi_hdr.h"
#include <stdlib.h>
#define BUF_SIZE 1024
#define NO_EINTR(stmt) while ((stmt) == -1 && errno == EINTR);

// 在进行系统调用的时候突然被 signal 打断 -> error EINTR -> 需要重新来
int main() {
    // 未初始化
    int fd;
    char* buf;
    int cnt;

    // 方法1：重新进行系统调用
    while ((cnt = read(fd, buf, BUF_SIZE)) == -1 && errno == EINTR)
        continue;
    if (cnt == -1) /* read() failed with other than EINTR */
    errExit("read");

    // 方法2：适用宏定义
    NO_EINTR(cnt = read(fd, buf, BUF_SIZE));

}