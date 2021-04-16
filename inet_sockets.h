#ifndef INET_SOCKETS_H
#define INET_SOCKETS_H /* Prevent accidental double inclusion */
#include <sys/socket.h>
#include <netdb.h>

// 自定义 socket library

// creates a socket with the given socket type, 
// and connects it to the address specified by host and service. 
// This function is designed for TCP or UDP client
// that need to connect their socket to a server socket.
// Returns a file descriptor on success, or –1 on error
int inetConnect(const char *host, const char *service, int type);

// creates a listening stream (SOCK_STREAM) socket bound
// to the wildcard IP address on the TCP port specified by service. 
// This function is designed for use by TCP servers.
// Returns a file descriptor on success, or –1 on error
int inetListen(const char *service, int backlog, socklen_t *addrlen);

// creates a socket of the given type, bound to the wildcard
// IP address on the port specified by service and type.
// (The socket type indicates whether this is a TCP or UDP service.) 
// This function is designed (primarily) for UDP servers and clients 
// to create a socket bound to a specific address.
// Returns a file descriptor on success, or –1 on error
int inetBind(const char *service, int type, socklen_t *addrlen);

// converts an Internet socket address to printable form
// Returns pointer to addrStr, a string containing host and service name
char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen,
                     char *addrStr, int addrStrLen);
#define IS_ADDR_STR_LEN 4096
/* Suggested length for string buffer that caller
 should pass to inetAddressStr(). Must be greater
 than (NI_MAXHOST + NI_MAXSERV + 4) */
#endif