#ifndef __config_h
#define __config_h

struct cdef_t {
    char key[33];
    char *value;
    struct cdef_t *next_def;
};

struct centry_t {
    char section[32];

    struct cdef_t  *def;
    struct centry_t *next_entry;
};

#define __config_get_entry(configobj, section, out) do {\
    typeof(configob) __tmp = (configobj);    \
    for (; __tmp != NULL; __tmp = __tmp->next_entry) \
        if (!memcmp(__tmp->section, section, sizeof(section)))  \
            out = __tmp; \
    out = NULL; \
} while (0)

#define __config_get_val(configentry, key, out, out_len) do { \
    typeof(configentry) __tmp = (configobj)->def; \
    for (; __tmp != NULL; __tmp = __tmp->next_def) {\
        if (!memcmp(__tmp->key, key, sizeof(key))) { \
            strncpy(out, __tmp->value, out_len); \
            break; \
        } \
    } \
    __out[out_len] = '\0'; \
} while (0)

#define __config_free(configobj) do { \
    typeof(configobj) __tmp, __p = (configobj);  \
    while (__p) { \
        __tmp = __p->next;  \
        free(__p);          \
        __p = __tmp;         \
    }  \
} while(0)

extern struct centry_t *config_parse(const char *filename);

#endif   /* __config_h */

#ifndef __config_h
#define __config_h

struct cdef_t {
    char key[33];
    char *value;
    struct cdef_t *next_def;
};

struct centry_t {
    char section[32];

    struct cdef_t  *def;
    struct centry_t *next_entry;
};

#define __config_get_entry(configobj, section, out) do {\
    typeof(configob) __tmp = (configobj);    \
    for (; __tmp != NULL; __tmp = __tmp->next_entry) \
        if (!memcmp(__tmp->section, section, sizeof(section)))  \
            out = __tmp; \
    out = NULL; \
} while (0)

#define __config_get_val(configentry, key, out, out_len) do { \
    typeof(configentry) __tmp = (configobj)->def; \
    for (; __tmp != NULL; __tmp = __tmp->next_def) {\
        if (!memcmp(__tmp->key, key, sizeof(key))) { \
            strncpy(out, __tmp->value, out_len); \
            break; \
        } \
    } \
    __out[out_len] = '\0'; \
} while (0)

#define __config_free(configobj) do { \
    typeof(configobj) __tmp, __p = (configobj);  \
    while (__p) { \
        __tmp = __p->next;  \
        free(__p);          \
        __p = __tmp;         \
    }  \
} while(0)

extern struct centry_t *config_parse(const char *filename);

#endif   /* __config_h */

