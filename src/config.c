#include "config.h"
#include "strmisc.h"

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>

struct centry_t *config_parse(const char *filename)
{
    FILE *fp;
    int current_line = 0,
        open_brace   = 0;
    int len, i;
    struct centry_t *ret = NULL;
    char line[2947], **tokens;

    fp = fopen(filename, "r");
    if (!fp)
        fatal("failed to open configuration file %s\n", filename);

    while (fgets(line, 2048, fp)) {
        current_line++;

        char *ptr = strchr(line, '#');
        if (ptr)
            *ptr = '\0';

        len = strlen(line);
        switch (open_brace) {
        case 0:
            for (i = 0; i < len; i++) {
                /* do we have a section? */
                if (line[i] == '{') {
                    open_brace = 1;
                    line[i] = 0;

                    /* can safely create a new config entry... */
                    struct centry_t *entry;
                    entry = malloc(sizeof(*entry));

                    strncpy(entry->section, strtrim(line), 32);

                    entry->section[32] = '\0';
                    entry->def = NULL;
                    entry->next_entry = ret;
                    ret = entry;
                    break;
                }
            }
            if (!open_brace)
                fatal("parser error: out of brace at line %d\n",
                        current_line);
            break;
        case 1:
            for (i = 0; i < len; i++) {
                if (line[i] == '}') {
                    open_brace = 0;
                    break;
                }
            }
            if (open_brace) {
                int n_tokens = 0;
                struct cdef_t *def;

                tokens = strexplode(line, '=', &n_tokens);
                if (!tokens || n_tokens>1)
                    fatal("parser error: illegal equality at line %d\n",
                            current_line);
 
                def = malloc(sizeof(*def));
                if (!def)
                    fatal("parser error: cannot allocate %zu bytes for config define\n",
                            sizeof(*def));

                if (n_tokens == 1) {
                    strncpy(def->key, strtrim(tokens[0]), 33);
                    def->key[32] = '\0';

                    def->value = strdup(strtrim(tokens[1]));
                } else
                    fatal("parser error: value with no key[?] at line %d\n",
                            current_line);

                def->next_def = ret->def;
                ret->def = def;
            }
            break;
        }
    }

    fclose(fp);
    return ret;
}

