#include "map.h"
#include "strmisc.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

struct pair *get_pair(struct bucket *bucket, const char *key);
unsigned long hash(const char *str);

void map_init(struct map* map) {
    map->buckets = malloc(map->count * sizeof(struct bucket));
    if (map->buckets == NULL) {
        fprintf(stderr,
                "fatal: failed to allocate buckets of count %u for map\n",
                map->count);
        exit(EXIT_FAILURE);
    }

    memset(map->buckets, 0, map->count * sizeof(struct bucket));
}

void map_free(struct map *map) {
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
    if (map->filename && remove(map->filename) == -1)
        printf("map_free(0x%p): warning: can't remove %s: (%d): %s\n", map, map->filename,
                errno, strerror(errno));
}

struct pair *map_get(const struct map *map, const char *key, char *out_buf,
        unsigned int n_out) {
    unsigned int index;
    struct bucket *bucket;
    struct pair *pair;

    if (!map)
        return NULL;
    if (!key)
        return NULL;

    index = hash(key) % map->count;
    bucket = &map->buckets[index];
    pair = get_pair(bucket, key);

    if (!pair)
        return NULL;

    if (!out_buf || !n_out)
        return NULL;
    if (strlen(pair->value) >= n_out)
        return NULL;
    strcpy(out_buf, pair->value);
    return pair;
}

bool map_exists(const struct map *map, const char *key) {
    unsigned int index;
    struct bucket *bucket;

    if (key == NULL)
        return false;

    index = hash(key) % map->count;
    bucket = &map->buckets[index];

    return get_pair(bucket, key);
}

bool map_unset(const struct map *map, const char *key) {
    unsigned int index;
    struct bucket *bucket;
    struct pair *pair;

    if (key == NULL)
        return false;

    index = hash(key) % map->count;
    bucket = &map->buckets[index];

    pair = get_pair(bucket, key);
    if (pair) {
        free(pair->key);
        free(pair->value);
        return true;
    }
    return false;
}

struct pair *map_put(const struct map *map, const char *key,
            const char* value) {
   unsigned int key_len, value_len, index;
   struct bucket *bucket;
   struct pair *tmp_pairs, *pair;
   char *tmp_value;
   char *new_key, *new_value;

    if (key == NULL || value == NULL)
        return NULL;

    key_len = strlen(key);
    value_len = strlen(value);

    index = hash(key) % map->count;
    bucket = &map->buckets[index];

    pair = get_pair(bucket, key);
    if (pair) {
        if (strlen(pair->value) < value_len) {
            tmp_value = realloc(pair->value, (value_len + 1) * sizeof(char));
            if (!tmp_value)
                return 0;

            pair->value = tmp_value;
        }
        strcpy(pair->value, value);
        return pair;
    }

    new_key = malloc((key_len + 1) * sizeof(char));
    if (!new_key)
        return NULL;

    new_value = malloc((value_len + 1) * sizeof(char));
    if (!new_value)
        goto out;

    if (bucket->count == 0) {
        bucket->pairs = malloc(sizeof(struct pair));    /* initial pair */
        if (bucket->pairs == NULL)
            goto out;

        bucket->count = 1;
    } else {
        tmp_pairs = realloc(bucket->pairs, (bucket->count + 1) * sizeof(struct pair));
        if (tmp_pairs == NULL)
            goto out;

        bucket->pairs = tmp_pairs;
        bucket->count++;
    }

    pair = &bucket->pairs[bucket->count - 1];
    pair->key = new_key;
    pair->value = new_value;

    strcpy(pair->key, key);
    strcpy(pair->value, value);

    return pair;
out:
    xfree(new_key);
    xfree(new_value);
    return NULL;
}

int map_get_count(const struct map *map) {
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
        while (j < m) {
            count++;
            pair++;
            j++;
        }
        bucket++;
        i++;
    }
    return count;
}

struct pair *get_pair(struct bucket *bucket, const char *key) {
    unsigned int i, n;
    struct pair *pair;

    n=bucket->count;
    if (n == 0)
        return NULL;

    pair = bucket->pairs;
    i = 0;
    while (i < n) {
        if (pair->key != NULL && pair->value != NULL) {
            if (!strcmp(pair->key, key) || !strwildmatch(key, pair->key))
                return pair;
        }
        pair++;
        i++;
    }
    return NULL;
}

unsigned long hash(const char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

