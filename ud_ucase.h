#include <sys/un.h>
#include <sys/socket.h>
#include <ctype.h>
#include "tlpi_hdr.h"
#define BUF_SIZE 10 /* Maximum size of messages exchange between client to server */
#define SV_SOCK_PATH "/tmp/ud_ucase"