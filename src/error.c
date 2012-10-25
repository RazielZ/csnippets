#include "error.h"
#include "asprintf.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

static const char *type_strings[] = { "FATAL", "WARNING", "NOTICE", NULL };
extern char *prog;

#define __log(s, args...)  \
    fprintf(stderr, s, ##args); /* freopen specific (stderr) */ \
    fprintf(stdout, s, ##args); /* on console */

void __noreturn error_nret(const char *str, ...)
{
    va_list ap;
    char *buff;

    va_start(ap, str);
    vasprintf(&buff, str, ap); 
    va_end(ap);

    __log(buff);
    __log("%s: error is not recoverable, terminating now...\n", prog);
    exit(EXIT_FAILURE);
}

void __noreturn log_errno(const char *str, ...)
{
    va_list ap;
    char *buff;
    int len;

    va_start(ap, str);
    len = vasprintf(&buff, str, ap);
    va_end(ap);

    __log("%s: %s (%d): %s\n", prog, buff, errno, strerror(errno));
    __log("%s: error is not recoverable, terminating now...\n", prog);
    exit(EXIT_FAILURE);
}

void error(int error_type, const char *str, ...)
{
    va_list va;
    char *buff;
    int len;

    va_start(va, str);
    len = vasprintf(&buff, str, va);
    va_end(va);

    __log("%s: %s: %s", prog, type_strings[error_type], buff);
}

