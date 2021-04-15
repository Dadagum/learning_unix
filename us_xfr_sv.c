#include "us_xfr.h"
#define BACKLOG 5

// 实例程序：Stream Sockets in the UNIX Domain
// server 端

int main(int argc, char *argv[])
{
    struct sockaddr_un addr;
    int sfd, cfd;
    ssize_t numRead;
    char buf[BUF_SIZE];

    // 创建 socket（tcp）
    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
        errExit("socket");
    // 先删除可能存在的 SV_SOCK_PATH
    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT)
        errExit("remove-%s", SV_SOCK_PATH);
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);
    // socket bind addr
    if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
        errExit("bind");
    // 监听请求
    if (listen(sfd, BACKLOG) == -1)
        errExit("listen");
    for (;;)
    { 
        cfd = accept(sfd, NULL, NULL);
        if (cfd == -1)
            errExit("accept");
        // 读取客户端传输过来的数据
        while ((numRead = read(cfd, buf, BUF_SIZE)) > 0)
            // 输出在控制台
            if (write(STDOUT_FILENO, buf, numRead) != numRead)
                fatal("partial/failed write");
        if (numRead == -1)
            errExit("read");
        if (close(cfd) == -1)
            errMsg("close");
    }
}