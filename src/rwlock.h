#ifndef __rwlock_h
#define __rwlock_h

typedef union rwticket rwticket;

union rwticket {
    unsigned u;
    unsigned short us;
    __extension__ struct {
        unsigned char write;
        unsigned char read;
        unsigned char users;
    } s;
};

extern void rwticket_wrlock(rwticket *l);
extern void rwticket_wrunlock(rwticket *l);
extern int rwticket_wrtrylock(rwticket *l);
extern void rwticket_rdlock(rwticket *l);
extern void rwticket_rdunlock(rwticket *l);
extern int rwticket_rdtrylock(rwticket *l);

#endif  /* __rwlock_h */

