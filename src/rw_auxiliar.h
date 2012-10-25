/* This header hsould be included by rwlock.c only */
#ifndef __rw_auxiliar_h
#define __rw_auxiliar_h

#define atomic_xadd(P, V)      __sync_fetch_and_add((P), (V))
#define cmpxchg(P, O, N)       __sync_val_compare_and_swap((P), (O), (N))
#define atomic_inc(P)          __sync_add_and_fetch((P), 1)

/* Compile read-write barrier */
#define barrier() asm volatile("":::"memory")

/* Pause instruction to prevent excess processor bus usage */
#define cpu_relax() asm volatile("pause\n":::"memory")

#endif /* __rw_auxiliar_h */

