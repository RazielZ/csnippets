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
#define log(str, ...)       error(LOG_NOTICE, str, __VA_ARGS__)
#define fatal(str, ...)     error_nret(str, __VA_ARGS__)
#define warning(str, ...)   error(LOG_WARNING, str, __VA_ARGS__)

/*
 * should be called at main().
 */
extern void log_init(void);
/*
 * should be called...  atexit() maybe.
 */
extern void log_term(void);

extern void __noreturn error_nret(const char *str, ...);
extern void __noreturn log_errno(const char *str, ...);
extern void error(int log_type, const char *str, ...);

#endif  /* __error_h */

