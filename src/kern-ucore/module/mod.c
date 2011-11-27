#include "mod.h"
#include <stdio.h>
#include <string.h>
#include <kio.h>

#define CHARS_MAX            10240
static uint32_t char_count;
static char     chars[CHARS_MAX];

#define EXPORT_SYM_COUNT_MAX 1024
#define EXPORT_SYM_HASH      197

static uint32_t    ex_sym_count;

static int         ex_sym_f[EXPORT_SYM_HASH];
static const char *ex_sym_name[EXPORT_SYM_COUNT_MAX];
static uintptr_t   ex_sym_ptr[EXPORT_SYM_COUNT_MAX];
static uint32_t    ex_sym_flags[EXPORT_SYM_COUNT_MAX];
static int         ex_sym_n[EXPORT_SYM_COUNT_MAX];

static void touch_export_sym(const char *name, uintptr_t ptr, uint32_t flags);
static uint32_t sym_hash(const char *name, uint32_t len);

/** 
 * init kerner-module system, setup symbol system, and export common symbols
 */

#define EXPORT(name) touch_export_sym(#name, (uintptr_t)&name, 0)

void
mod_init() {
    kprintf("mod_init\n");
    ex_sym_count = 0;
    char_count = 0;
    
    memset(ex_sym_f, -1, sizeof(ex_sym_f));

    EXPORT(kprintf);

}

static void touch_export_sym(const char *name, uintptr_t ptr, uint32_t flags) {
    int name_len = strlen(name);
    int h = sym_hash(name, name_len);
    int cur = ex_sym_f[h];

    while (cur != -1) {
        if (strcmp(ex_sym_name[cur], name) == 0) {
            // symbol found
            break;
        } else {
            cur = ex_sym_n[cur];
        }
    }
    // not found, create one
    if (cur == -1) {
        cur = ex_sym_count++;
        ex_sym_n[cur] = ex_sym_f[h];
        ex_sym_f[h] = cur;

        char * _name = chars + char_count;
        memmove(_name, name, name_len);
        char_count += name_len;
        ex_sym_name[cur] = _name;
        chars[char_count++] = 0;
    }

    ex_sym_ptr[cur] = ptr;
    ex_sym_flags[cur] = flags;
}

static uint32_t sym_hash(const char *name, uint32_t len) {
    int i;
    uint32_t ret = 0;
    for (i = 0; i != len; i++) {
        ret = (ret * 13 + name[i]) % EXPORT_SYM_HASH;
    }
    return ret;
}
