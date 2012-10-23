#ifndef __map_h
#define __map_h

struct pair {
    char *key;
    char *value;
};

struct bucket {
    unsigned int count;
    struct pair *pairs;
};

struct map {
    unsigned int count;
    struct bucket *buckets;
    const char *filename;
};

#define MAP_INITIALIZER { 1,  NULL, NULL }

extern void map_init(struct map *map);
extern void map_free(struct map *map);

extern struct pair *map_get(const struct map *map,
        const char *key, char *out_buf, unsigned int n_out);
extern bool map_exists(const struct map *map, const char *key);
extern bool map_unset(const struct map* map, const char *key);
extern struct pair *map_put(const struct map *map,
        const char *key, const char *value);
extern int map_get_count(const struct map* map);

#endif   /* __map_h */

