#include "error.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#ifndef USE_FREOPEN
static FILE *log = NULL;
#endif
static const char *type_strings[] = { "FATAL", "WARNING", "NOTICE", NULL };
extern char *prog;

void log_init(void)
{
#ifdef USE_FREOPEN
    freopen(LOG_FILE, "w", stderr);
#else
    log = fopen(LOG_FILE, "w");
    if (!log) {
        fprintf(stderr, "failed to open file " LOG_FILE " (%d): %s\n",
                errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
#endif
}

void log_term(void)
{
#ifndef USE_FREOPEN
    fclose(log);
    log = NULL;
#endif
}

void __noreturn error_nret(const char *str, ...)
{
    va_list ap;

    va_start(ap, str);
    vfprintf(stderr, str, ap);
    va_end(ap);

    fprintf(stderr, "%s: error is not recoverable, terminating now...\n", prog);
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

#ifndef USE_FREOPEN
    if (log)
        fprintf(log, "%s: %s (%d): %s\n",
                prog, buff, errno, strerror(errno));
#endif
    fprintf(stderr, "%s: %s (%d): %s\n",
        prog, buff, errno, strerror(errno));
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

#ifndef USE_FREOPEN
    if (log)
        fprintf(log, "%s: %s: %s", prog, type_strings[error_type], buff);
#endif
    fprintf(stderr, "%s: %s: %s", prog, type_strings[error_type], buff);
}

