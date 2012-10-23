/*
 * This code is licensed under BSD-MIT By Rusty Russell, see also:
 * https://github.com/rustyrussell/ccan/
 */

#ifndef __asprintf_h
#define __asprintf_h

#include <stdarg.h>

extern char *__printf(1, 2) afmt(const char *fmt, ...);
extern int __printf(2, 3) asprintf(char **strp, const char *fmt, ...);
extern int vasprintf(char **strp, const char *fmt, va_list va);

#endif  /* __asprintf_h */

