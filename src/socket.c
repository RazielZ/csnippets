#include "socket.h"

#define _GNU_SOURCE
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void setup_async(int fd) {
    int old_flags;

    old_flags = fcntl(fd, F_GETFL, 0);
    if (!(old_flags & O_NONBLOCK))
        old_flags |= O_NONBLOCK;

    fcntl(fd, F_SETFL, old_flags);
}

int get_sock(const char *addr, int port, int is_async) {
    int sockfd;
    struct sockaddr_in srv;
    struct hostent *hp;
    time_t start;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        goto out;

    if (is_async)
        setup_async(sockfd);

    if (!(hp = gethostbyname(addr)))
        goto out;

    bcopy((char *)hp->h_addr, (char *)&srv.sin_addr, hp->h_length);

    srv.sin_family = AF_INET;
    srv.sin_port = htons(6667);

    start = time(NULL);
    while (time(NULL) - start < 10) {
        errno = 0;
        printf("Trying %s...\n", addr);
        if (connect(sockfd, (struct sockaddr *)&srv, sizeof(srv)) == -1) {
            switch (errno) {
                case EISCONN:
                case EALREADY:
                case EINPROGRESS:
                    setsockopt(sockfd, SOL_SOCKET, SO_LINGER, 0, 0);
                    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 0, 0);
                    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, 0, 0);
                    return sockfd;
                default:
                    continue;
            }
        }
    }

out:
    warning("failed to resolve %s: %s\n", addr, strerror(errno));
    if (sockfd != -1)
        close(sockfd);
    fatal("failed to connect to %s: %s\n", addr, strerror(errno));
}

int sends(int fd, char *buf, ...)
{
    char *buff;
    int len;
    int written;

    va_list va;

    va_start(va, buf);
    len = vasprintf(&buff, buf, va);
    va_end(va);

    written = write(fd, buff, len);
    free(buff);

    return written;
}

