#ifndef __map_h
#define __map_h

typedef unsigned long (*hash_function) (const char *);
typedef bool (*hash_compare) (const void *v1, const void *v2);

struct pair {
    char *key;
    void *value;
    unsigned int ref;
};

struct bucket {
    unsigned int count;
    struct pair *pairs;
};

struct map {
    unsigned int count;
    struct bucket *buckets;
    hash_function hash_function;
    hash_compare  hash_comp;
};

#define MAP_INIT { 1, NULL, NULL, NULL }
/*
 * some helper macros for `pair'
 */
#define PAIR_KEY(pair) (pair)->key
#define PAIR_VALUE(pair, cast) (cast)(pair)->value

extern void map_init(struct map *map);
extern void map_free(struct map *map);

extern struct pair *map_get(const struct map *map, const char *key);
extern bool map_has(const struct map *map, const char *key);
extern bool map_remove(const struct map* map, const char *key);
extern struct pair *map_put(const struct map *map, const char *key, void *value);
extern int map_get_count(const struct map* map);

#endif   /* __map_h */

