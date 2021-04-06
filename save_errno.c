#include <unistd.h>
#include <signal.h>
#include "tlpi_hdr.h"

void handler(int sig) {
    // 保存主程序中的 errno
    int savedErrno;
    savedErrno = errno;

    /* Now we can execute a function that might modify errno */

    // 恢复 errno
    errno = savedErrno;
}

int main() {

}