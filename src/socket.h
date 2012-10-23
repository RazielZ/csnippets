#ifndef __socket_h
#define __socket_h

extern int __warn_unused get_sock(const char *addr, int port, int is_async);
extern int sends(int fd, char *buf, ...);

#endif    /* __socket_h */

