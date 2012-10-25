#include "config.h"
#include "asprintf.h"
#include "map.h"

#include <time.h>
#include <stdio.h>
#include <assert.h>

char *prog;
int main(int argc, char **argv)
{
    char *s;
    struct map map = MAP_INIT;
    map_init(&map);
    prog = argv[0];
    log_init();
    if (asprintf(&s, "%d %s", argc, prog) < 0)
        fatal("asprintf() failed: %d %s\n", argc, prog);

    struct pair *pair = map_put(&map, "hello", (void *)"world");
    assert(map_has(&map, "hello"));

    warning("%s (pair --> %s %s)\n", s, PAIR_KEY(pair), PAIR_VALUE(pair, char *));;
    assert(map_remove(&map, "hello"));
    assert(!map_has(&map, "hello"));
    fatal("we're done with the tests!\n");
}

