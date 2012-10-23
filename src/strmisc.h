#ifndef __str_misc_h
#define __str_misc_h

#include <stdbool.h>

extern char *strltrim(char *s);
extern char *strrtrim(char *s);
extern char *strtrim(char *s);
extern char **strexplode(char *string, char seperator, int *size);
extern int strwildmatch(const char *pattern, const char *string);
extern bool strupper(char *str);

#endif /*  __str_misc_h */

