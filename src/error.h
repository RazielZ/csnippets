#ifndef __error_h
#define __error_h

/*
 * WARNING: Do not change these numbers as they're used as `index'
 * for `type_strings' see error.c fore more information.
 */
#define LOG_FATAL     0
#define LOG_WARNING   1
#define LOG_NOTICE    2

#define LOG_FILE "error.log"    /* log file to print to.  */
/* shortcuts. */
#define log(str, args...)       error(LOG_NOTICE, str, ##args)
#define fatal(str, args...)     error_nret(str, ##args)
#define warning(str, args...)   error(LOG_WARNING, str, ##args)

#define log_init()          freopen(LOG_FILE, "w", stderr);

extern void __noreturn error_nret(const char *str, ...);
extern void __noreturn log_errno(const char *str, ...);
extern void error(int log_type, const char *str, ...);

#endif  /* __error_h */

