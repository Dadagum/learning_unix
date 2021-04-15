#include "i6d_ucase.h"

// 示例程序：采用 udp 进行 case conversion 服务程序（采用 ipv6）
// 服务端程序

int main(int argc, char *argv[])
{
    struct sockaddr_in6 svaddr, claddr;
    int sfd, j;
    ssize_t numBytes;
    socklen_t len;
    char buf[BUF_SIZE];
    char claddrStr[INET6_ADDRSTRLEN]; // client ipv6 地址

    // 创建 socket
    sfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sfd == -1)
        errExit("socket");

    // 构建服务端地址
    memset(&svaddr, 0, sizeof(struct sockaddr_in6));
    svaddr.sin6_family = AF_INET6; // ipv6
    svaddr.sin6_addr = in6addr_any; // Wildcard address：主机上所有可用网卡
    svaddr.sin6_port = htons(PORT_NUM); // 端口号

    // socket 绑定地址
    if (bind(sfd, (struct sockaddr *)&svaddr,
             sizeof(struct sockaddr_in6)) == -1)
        errExit("bind");

    /* Receive messages, convert to uppercase, and return to client */
    for (;;)
    {
        len = sizeof(struct sockaddr_in6);
        // 接受客户端来的数据，并记录地址信息
        numBytes = recvfrom(sfd, buf, BUF_SIZE, 0,
                            (struct sockaddr *)&claddr, &len);
        if (numBytes == -1)
            errExit("recvfrom");
        // 顺便也演示一下转换函数：将客户端的 ip 地址转换为字符串类型，控制台打印出来
        if (inet_ntop(AF_INET6, &claddr.sin6_addr, claddrStr,
                      INET6_ADDRSTRLEN) == NULL)
            printf("Couldn't convert client address to string\n");
        else
            printf("Server received %ld bytes from (%s, %u)\n",
                   (long)numBytes, claddrStr, ntohs(claddr.sin6_port));
        // 进行大小写转换
        for (j = 0; j < numBytes; j++)
            buf[j] = toupper((unsigned char)buf[j]);
        // 将转换结果发送给客户端
        if (sendto(sfd, buf, numBytes, 0, (struct sockaddr *)&claddr, len) !=
            numBytes)
            fatal("sendto");
    }
}