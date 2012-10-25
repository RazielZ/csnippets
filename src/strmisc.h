#ifndef __str_misc_h
#define __str_misc_h

extern char *strtrim(char *str);
extern char **strexplode(char *string, char seperator, int *size);
extern int strwildmatch(const char *pattern, const char *string);
extern bool strupper(char *str);

#endif /*  __str_misc_h */

