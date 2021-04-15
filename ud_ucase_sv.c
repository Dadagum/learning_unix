#include "ud_ucase.h"

// 实例程序： Datagram Sockets in the UNIX Domain
// server 端

int main(int argc, char *argv[])
{
    struct sockaddr_un svaddr, claddr;
    int sfd, j;
    ssize_t numBytes;
    socklen_t len;
    char buf[BUF_SIZE];
    // 创建 socket（udp）
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0); /* Create server socket */
    if (sfd == -1)
        errExit("socket");
    /* Construct well-known address and bind server socket to it */
    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT)
        errExit("remove-%s", SV_SOCK_PATH);

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);
    // socket bind addr
    if (bind(sfd, (struct sockaddr *)&svaddr, sizeof(struct sockaddr_un)) == -1)
        errExit("bind");
    /* Receive messages, convert to uppercase, and return to client */
    for (;;)
    {
        // 使用 recvfrom 接受数据报，并且接受得到客户端的地址信息
        len = sizeof(struct sockaddr_un);
        numBytes = recvfrom(sfd, buf, BUF_SIZE, 0,
                            (struct sockaddr *)&claddr, &len);
        if (numBytes == -1)
            errExit("recvfrom");
        printf("Server received %ld bytes from %s\n", (long)numBytes,
               claddr.sun_path);
        // 将数据转换成大写，然后重新发送给客户端
        for (j = 0; j < numBytes; j++)
            buf[j] = toupper((unsigned char)buf[j]);
        if (sendto(sfd, buf, numBytes, 0, (struct sockaddr *)&claddr, len) !=
            numBytes)
            fatal("sendto");
    }
}