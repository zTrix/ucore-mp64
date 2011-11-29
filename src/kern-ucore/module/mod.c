#include "mod.h"
#include <stdio.h>
#include <string.h>
#include <kio.h>
#include <file.h>
#include <stat.h>
#include <slab.h>
#include <elf.h>

#define error(x ...) kprintf("[ EE ] %s:%d, ", __FILE__, __LINE__);kprintf(x)

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
static int elf_head_check(void * elf);

void load_mod_test();

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

    kprintf("mod_init done\n");
    
    uintptr_t mod_test_addr;
    uint32_t mod_test_size;
    load_mod_test(&mod_test_addr, &mod_test_size);

    struct elf_mod_info_s info;
    elf_mod_load(mod_test_addr, mod_test_size, &info);
}

void load_mod_test(uintptr_t * addr, uint32_t * size) {
    kprintf("loading kern-module test\n");
    char test_file_path[] = "/kern-module/mod-test.o";
    int test_fd = file_open(test_file_path, O_RDONLY);
    struct stat mod_stat;
    memset(&mod_stat, 0, sizeof(mod_stat));
    file_fstat(test_fd, &mod_stat);
    if (mod_stat.st_size <= 0 || mod_stat.st_size > (1<<20)) {
        kprintf("wrong obj file size: %d\n", mod_stat.st_size);
        return;
    }
    kprintf("loading test kern module, size = %d\n", mod_stat.st_size);
    void * buffer = kmalloc(mod_stat.st_size + 1);
    if (buffer == NULL) {
        panic("load mod test failed: insufficient memory");
    }
    size_t copied;
    file_read(test_fd, buffer, mod_stat.st_size, &copied);
    kprintf("obj mem: %x, size is %d\n", buffer, copied);
    *addr = (uintptr_t)buffer;
    *size = mod_stat.st_size;
}

int elf_mod_load(uintptr_t image, uint32_t image_size, struct elf_mod_info_s * info) {
    info->image = image;
    info->image_size = image_size;

    if (elf_head_check((void *)image)) {
        return -1;
    }
    // stack 0
    return 0;
}

static int elf_head_check(void * elf) {
    struct elfhdr * eh = (struct elfhdr *)elf;
    if (eh->e_magic != ELF_MAGIC) {
        error("invalid signature: %x\n", eh->e_magic);
        return -1;
    }

    if (eh->e_elf[0] != 0x2) {
        error("not ELFCLASS64");
        return -1;
    }

    if (eh->e_type != 0x01) {       // ET_REL , Relocatable object file
        error("error type: %d\n", eh->e_type);
        return -1;
    }

    if (eh->e_machine != 0x3e) {    // AMD x86-64 architecture
        error("error machine type: %d \n", eh->e_machine);
        return -1;
    }

    if (eh->e_entry != 0x0) {
        error("error entry code: %d\n", eh->e_entry);
        return -1;
    }
    
    kprintf("[ OK ] elf head check passed !\n");
    return 0;
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
