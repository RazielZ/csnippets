#include "mod.h"
#include "strmisc.h"

#include <elf.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#define _GNU_SOURCE
#include <dlfcn.h>

static __inline int filter(const struct dirent *dir) {
    const char *dname = dir->d_name;
    return strlen(dname) > 3 ? !strcmp(dname+strlen(dname) - 3, ".so") : 0;
}

static bool is_event_in_chain(char *name, event_t **event_chain,
        int event_count)
{
    int i;

    for (i = 0; i < event_count; i++)
        if (strcmp((*event_chain)[i].event_name, name) == 0)
            return true;
    return false;
}

static int add_event_to_chain(char *name, event_t **chain,
        int event_count)
{
    *chain = realloc(*chain, (event_count + 1) * sizeof(event_t));
    if (!*chain)
        return -errno;

    (*chain)[event_count].event_name = malloc(strlen(name) + 1);
    if (!(*chain)[event_count].event_name)
        return -ENOMEM;

    strcpy((*chain)[event_count].event_name,
            name);
    (*chain)[event_count].event_handlers = NULL;
    (*chain)[event_count].handlers_count = 0;
    return 0;
}

static int add_symbol_to_chain(char *func_name, char *cur_event_name,
        const char *module_name, void *module_handle, event_t **event_chain,
        int event_count)
{
    int i;
    event_handler_t handler, *handlers;
    char *evt_name;
    int count;

    for (i = 0; i < event_count; i++) {
        evt_name = (*event_chain)[i].event_name;
        count = (*event_chain)[i].handlers_count;

        if (strcmp(evt_name, cur_event_name) == 0) {
            handlers = (*event_chain)[i].event_handlers;
            handlers = realloc(handlers, (count + 1)  * sizeof(struct event_handler));
            handler = handlers[(*event_chain)[i].handlers_count];

            handler.func = dlsym(module_handle, func_name);
            strcpy(handler.module_name, module_name);
         }
    }

    return 0;
} 

static int load_event_handlers(const char *filename, event_t **event_chain, int *no_events)
{
    FILE *fp;
    char *buffer, **current_symbols,
         **current_events;
    int file_size = 0, i, j, symbolstrtbl,
        current_symbol_count = 0;
    int err;

    Elf32_Ehdr *hdr;
    Elf32_Shdr *shdr;
    Elf32_Sym  *sym;
    void *module_handle;

    fp = fopen(filename, "r");
    if (!fp) 
        return errno;

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    buffer = malloc(file_size);
    if (!buffer) {
        err = ENOMEM;
        goto out;
    }

    if (fread(buffer, 1, file_size, fp) != file_size) {
        err = ENOBUFS;
        goto out;
    }

    fclose(fp);

    hdr = (Elf32_Ehdr*)buffer;
    shdr = (Elf32_Shdr*)(buffer + hdr->e_shoff);

    for (i = 0; i < hdr->e_shnum; i++) {
        if (shdr[i].sh_type == SHT_STRTAB) {
            if (strcmp(buffer + shdr[hdr->e_shstrndx].sh_offset+shdr[i].sh_name,
                        ".dynstr") == 0) {
                symbolstrtbl = i;
                break;
            }
        }
    }

    for (i = 0; i < hdr->e_shnum; i++) {
        if (shdr[i].sh_type == SHT_DYNSYM) {
            sym = (Elf32_Sym *)(buffer+shdr[i].sh_offset);
            for (j = 0; j < shdr[i].sh_size/shdr[i].sh_entsize; j++) {
                if (!sym[j].st_name)
                    continue;
                if (!sym[j].st_size)
                    continue;
                char func_name[256] = {0}, *ptr,
                     *original = buffer+shdr[symbolstrtbl].sh_offset+sym[j].st_name;
                strcpy(func_name, original);
                ptr = strtok(func_name, "_");

                if (func_name != ptr)
                    continue;
                if (!strupper(ptr))
                    continue;

                current_symbols = realloc(current_symbols,
                        (current_symbol_count + 1) * sizeof(char *));
                if (!current_symbols)
                    return ENOMEM;

                current_symbols[current_symbol_count] = malloc(strlen(original) + 1);
                if (!current_symbols)
                    return ENOMEM;
                strcpy(current_symbols[current_symbol_count], original);
                current_events = realloc(current_events,
                        (current_symbol_count + 1) * sizeof(char*));
                current_symbol_count++;
            }
        }
    }

    char tempbuf[256] = {0};
    sprintf(tempbuf, "./%s", filename);

    module_handle = dlopen(tempbuf, RTLD_LAZY);
    if (!module_handle)
        return errno;

    for (i = 0; i < current_symbol_count; i++) {
        if (!is_event_in_chain(current_events[i], event_chain, *no_events)) {
            add_event_to_chain(current_events[i], event_chain, *no_events);
            (*no_events)++;
        }
        add_symbol_to_chain(current_symbols[i], current_events[i],
                filename, module_handle, event_chain, *no_events);
    }

    for (i = 0; i < current_symbol_count; i++) {
        free(current_symbols[i]);
        free(current_events[i]);
    }

    free(current_symbols);
    free(current_events);
    free(buffer);
    return 0;
out:
    fclose(fp);
    return err;
}

int load_modules(event_t **event_chain)
{
    int no_events = 0;
    int socount = 0;
    int ret = 0;
    struct dirent **solist;

    socount = scandir("mods", &solist, filter, NULL);
    while (socount--) {
        if ((ret = load_event_handlers(solist[socount]->d_name, event_chain,
                        &no_events) != 0)) {
            fprintf(stderr, "failed to load event %s (%d): %s\n",
                    solist[socount]->d_name, ret, strerror(ret));;
        }
        free(solist[socount]);
    }
    free(solist);
    return no_events;
}

