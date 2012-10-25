#ifndef __rwlock_h
#define __rwlock_h

typedef union rwlock rwlock_t;

union rwlock {
    unsigned u;
    unsigned short us;
    __extension__ struct {
        unsigned char write;
        unsigned char read;
        unsigned char users;
    } s;
};

extern void rwlock_wrlock(rwlock_t *l);
extern void rwlock_wrunlock(rwlock_t *l);
extern int rwlock_wrtrylock(rwlock_t *l);
extern void rwlock_rdlock(rwlock_t *l);
extern void rwlock_rdunlock(rwlock_t *l);
extern int rwlock_rdtrylock(rwlock_t *l);

#endif  /* __rwlock_h */

