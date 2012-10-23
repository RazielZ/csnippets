#ifndef __linux_style_h
#define __linux_style_h

#ifdef __GNUC__
#define __noreturn      __attribute((noreturn))
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#define __inline        __attribute((always_inline))
#define __warn_unused   __attribute((warn_unused_result))
#define __const         __attribute((const))
#define __printf(nfmt, narg) \
    __attribute((format(__printf__, nfmt, narg)))
#else
#define __noreturn
#define likely(x) (x)
#define unlikely(x) (x)
#define __inline inline
#define __warn_unused
#define __const const
#define __printf(nfmt, narg)
#endif

#endif    /* __linux_style_h */

