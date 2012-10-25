#include <errno.h>
#include "rw_auxiliar.h"
#include "rwlock.h"

void rwlock_wrlock(rwlock_t *l)
{
    unsigned me = atomic_xadd(&l->u, (1<<16));
    unsigned char val = me >> 16;

    while (val != l->s.write) cpu_relax();
}

void rwlock_wrunlock(rwlock_t *l)
{
    rwlock_t t = *l;

    barrier();

    t.s.write++;
    t.s.read++;

    *(unsigned short *)l = t.us;
}

int rwlock_wrtrylock(rwlock_t *l)
{
    unsigned me = l->s.users;
    unsigned char menew = me + 1;
    unsigned read = l->s.read << 8;
    unsigned cmp = (me << 16) + read + me;
    unsigned cmpnew = (menew << 16) + read + me;

    if (cmpxchg(&l->u, cmp, cmpnew) == cmp)
        return 0;
    return EBUSY;
}

void rwlock_rdlock(rwlock_t *l)
{
    unsigned me = atomic_xadd(&l->u, (1<<16));
    unsigned char val = me >> 16;

    while (val != l->s.read) cpu_relax();
    l->s.read++;
}

void rwlock_rdunlock(rwlock_t *l)
{
    atomic_inc(&l->s.write);
}

int rwlock_rdtrylock(rwlock_t *l)
{
    unsigned me = l->s.users;
    unsigned write = l->s.write;
    unsigned char menew = me + 1;
    unsigned cmp = (me << 16) + (me << 8) + write;
    unsigned cmpnew = ((unsigned) menew << 16) + (menew << 8) + write;

    if (cmpxchg(&l->u, cmp, cmpnew) == cmp)
        return 0;
    return EBUSY;
}

