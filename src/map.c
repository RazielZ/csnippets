#include "map.h"
#include "strmisc.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define ref(pair) (pair)->ref++
#define deref(pair) (pair)->ref--;
#define hasref(pair) !!(pair)->ref
static struct pair *get_pair(const struct map *map, const char *key);

static unsigned long hash(const char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

static bool hash_cmp(const char *v1, const char *v2)
{
    return v1 && !strcmp(v1, v2);
}

void map_init(struct map* map)
{
    map->buckets = malloc(map->count * sizeof(struct bucket));
    if (!map->hash_function)
        map->hash_function = (hash_function) hash;
    if (!map->hash_comp)
        map->hash_comp = (hash_compare) hash_cmp;

    if (map->buckets == NULL)
        fatal("fatal: failed to allocate buckets of count %u for map buckets!\n", map->count);

    memset(map->buckets, 0, map->count * sizeof(struct bucket));
}

void map_free(struct map *map)
{
    unsigned i, j, n, m;
    struct bucket *bucket;
    struct pair *pair;

    if (map == NULL)
        return;

    n = map->count;
    bucket = map->buckets;
    i = 0;

    while (i < n) {
        m = bucket->count;
        pair = bucket->pairs;

        j= 0;
        while (j < m) {
            xfree(pair->key);
            xfree(pair->value);
            pair++;
            j++;
        }

        xfree(bucket->pairs);
        bucket++;
        i++;
    }
    xfree(map->buckets);
}

struct pair *map_get(const struct map *map, const char *key)
{
    return get_pair(map, key);
}

bool map_has(const struct map *map, const char *key)
{
    return !!get_pair(map, key);
}

bool map_remove(const struct map *map, const char *key)
{
    struct pair *pair;

    pair = get_pair(map, key);
    if (unlikely(!pair))
        return false;

    xfree(pair->key);
    return true; 
}

struct pair *map_put(const struct map *map, const char *key, void* value)
{
    unsigned int key_len;
    struct bucket *bucket;
    struct pair *tmp_pairs, *pair;
    char *new_key;

    key_len = strlen(key);
    pair = get_pair(map, key);

    if (likely(pair && pair->value == value))
        return pair;

    new_key = malloc((key_len + 1) * sizeof(char));
    if (unlikely(!new_key))
        return NULL;

    bucket = map->buckets;
    if (bucket->count == 0) {
        bucket->pairs = malloc(sizeof(struct pair));    /* initial pair */
        if (!bucket->pairs)
            goto out;

        bucket->count = 1;
    } else {
        xrealloc(tmp_pairs, bucket->pairs, (bucket->count + 1) * sizeof(struct pair),
                goto out);
        bucket->pairs = tmp_pairs;
        bucket->count++;
    }

    pair = &bucket->pairs[bucket->count - 1];
    pair->key = new_key;
    pair->value = value;
    ref(pair);

    strcpy(pair->key, key);
    return pair;
out:
    xfree(new_key);
    return NULL;
}

int map_get_count(const struct map *map)
{
    unsigned int i, j, n, m;
    unsigned int count;
    struct bucket *bucket;
    struct pair *pair;

    if (!map)
        return 0;

    bucket = map->buckets;
    n = map->count;
    i = 0;
    count = 0;

    while (i < n) {
        pair = bucket->pairs;
        m = bucket->count;
        j = 0;
        while (j < m && hasref(pair)) {
            count++;
            pair++;
            j++;
        }
        bucket++;
        i++;
    }
    return count;
}

static struct pair *get_pair(const struct map *map, const char *key)
{
    unsigned int i, n, index;
    struct pair *pair;
    struct bucket *bucket;

    index = map->hash_function(key) % map->count;
    bucket = &map->buckets[index];
    n = bucket->count;
    if (n == 0)
        return NULL;

    pair = bucket->pairs;
    for (i = 0; i < n; i++, pair++)
        if (hasref(pair) && map->hash_comp(pair->key, key))
            return pair;
    return NULL;
}

