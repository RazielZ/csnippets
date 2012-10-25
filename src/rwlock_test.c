#include "rwlock.h"

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

typedef struct {
    unsigned int data[10];
    rwlock_t lock;
} threadata_t;
char *prog;

static void checked(int rc) {
    if (rc) {
        fprintf(stderr,
                "fatal: function call failed: %d\n", rc);
        exit(EXIT_FAILURE);
    }
}

static void *readthread(void *p) {
    threadata_t *d = (threadata_t *)p;
    int i=0;
    rwlock_rdlock(&d->lock);
    log("read\n");
    for (i = 0; i < 10; i++)
        printf("%d\n", d->data[i]);
    rwlock_rdunlock(&d->lock);
    log("Done read\n");
    return NULL;
}

static void *writethread(void *p) {
    int i;
    threadata_t *d = (threadata_t *)p;
    log("write\n");
    for (i = 0; i < 10; i++) {
        int j = rand() % (i + 1) + 1;
        d->data[i] = j;
    }
    rwlock_wrunlock(&d->lock);
    log("Done write\n");
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t threads[2];
    threadata_t *td;
    xmalloc(td, sizeof(*td), return 0);

    srand(time(NULL));
    log_init();

    prog = argv[0];

    rwlock_wrlock(&td->lock);
    checked(pthread_create(&threads[0], NULL, writethread, td));
    checked(pthread_create(&threads[1], NULL, readthread, td));

    pthread_exit(NULL);
    fatal("rwlock tests done!\n");
}

