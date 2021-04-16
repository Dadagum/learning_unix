#include <syslog.h>
#include "id_echo.h"
#include "become_daemon.h"

// 采用迭代模式的 udp echo service

int main(int argc, char *argv[])
{
    int sfd;
    ssize_t numRead;
    socklen_t addrlen, len;
    struct sockaddr_storage claddr;
    char buf[BUF_SIZE];
    char addrStr[IS_ADDR_STR_LEN];
    // 变成 daemon 
    if (becomeDaemon(0) == -1)
        errExit("becomeDaemon");
    sfd = inetBind(SERVICE, SOCK_DGRAM, &addrlen);
    if (sfd == -1)
    {
        syslog(LOG_ERR, "Could not create server socket (%s)", strerror(errno));
        exit(EXIT_FAILURE);
    }
    /* Receive datagrams and return copies to senders */
    for (;;)
    {
        // 接受客户端的数据然后立刻返回
        len = sizeof(struct sockaddr_storage);
        numRead = recvfrom(sfd, buf, BUF_SIZE, 0,
                           (struct sockaddr *)&claddr, &len);
        if (numRead == -1)
            errExit("recvfrom");
        if (sendto(sfd, buf, numRead, 0, (struct sockaddr *)&claddr, len) != numRead)
            syslog(LOG_WARNING, "Error echoing response to %s (%s)",
                   inetAddressStr((struct sockaddr *)&claddr, len,
                                  addrStr, IS_ADDR_STR_LEN),
                   strerror(errno));
    }
}