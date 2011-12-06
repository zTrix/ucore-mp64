#include <stdio.h>
#include <string.h>
#include <kio.h>
#include <file.h>
#include <stat.h>
#include <slab.h>
#include <elf.h>
#include <bitsearch.h>
#include <mmu.h>
#include <vmm.h>
#include <mod_loader.h>
#include <mod.h>
#include <error.h>

#define EXPORT(name) touch_export_sym(#name, (uintptr_t)&name, 0)
typedef void (*voidfunc)();

static unsigned char ko_pool[10240];
static uintptr_t ko_pool_pointer;

void register_mod_add(func_add_t f) {
    mod_touch_symbol(MOD_ADD, (void *)f, 0);
}

void unregister_mod_add() {

}

void register_mod_mul(func_mul_t f) {
    mod_touch_symbol(MOD_MUL, (void *)f, 0);
}

void unregister_mod_mul() {

}

int load_mod_file(char *name, uintptr_t *addr, uint32_t *size) {
    int fd = file_open(name, O_RDONLY);
    if (fd < 0) {
        kprintf("[ EE ] cannot find kernel object file: %s\n", name);
        return -1;
    }
    struct stat mod_stat;
    memset(&mod_stat, 0, sizeof(mod_stat));
    file_fstat(fd, &mod_stat);
    if (mod_stat.st_size <= 0 || mod_stat.st_size > (1<<20)) {
        kprintf("[ EE ] wrong obj file size: %d\n", mod_stat.st_size);
        return -1;
    }
    kprintf("[ II ] loading kern module: %s, size = %d\n", name, mod_stat.st_size);
    void * buffer = (void *)ko_pool_pointer;
    if (buffer == NULL) {
        kprintf("[ EE ] not enough memory to load the module\n");
        return -1;
    }
    size_t copied;
    file_read(fd, buffer, mod_stat.st_size, &copied);
    kprintf("[ II ] module size: %d, mem addr: %x\n", copied, buffer);
    ko_pool_pointer += copied;
    *addr = (uintptr_t)buffer;
    *size = mod_stat.st_size;
    return 0;
}

int load_module(char * name) {
    int ret = 0;
    uintptr_t mod_addr;
    uint32_t mod_size;
    if ((ret = load_mod_file(name, &mod_addr, &mod_size)) != 0) {
        return ret;
    }

    struct elf_mod_info_s info;
    elf_mod_load(mod_addr, mod_size, &info);

    ((voidfunc)info.load_ptr)();
    return ret;
}

void mod_init() {
    kprintf("[ II ] mod_init\n");
    ko_pool_pointer = (uintptr_t)ko_pool;

    mod_loader_init();

    kprintf("[ II ] exporting kernel symbols\n");

    EXPORT(kprintf);
    EXPORT(register_mod_add);
    EXPORT(unregister_mod_add);
    EXPORT(register_mod_mul);
    EXPORT(unregister_mod_mul);
    
    // TODO: read mod dep file
   
    kprintf("[ II ] mod_init done\n");
}

#define MX_MOD_PATH_LEN 1024
static char tmp_path[MX_MOD_PATH_LEN];

uint64_t do_init_module(const char *name) {
    int ret;

    struct mm_struct *mm = current->mm;
    lock_mm(mm);
    int size = strlen(name);
    if (!copy_from_user(mm, tmp_path, name, size, 1)) {
        unlock_mm(mm);
        return -E_INVAL;
    }
    unlock_mm(mm);
    tmp_path[size] = '\0';
    
    ret = load_module(tmp_path);
    return ret;
}

int do_mod_add(int a, int b) {
    int c = 0;
    int idx = find_export_sym(MOD_ADD, 0);
    if (idx < 0) {
        kprintf("[ EE ] module add not loaded into kernel\n");
        return 0;
    }
    ((func_add_t)get_sym_ptr(idx))(a, b, &c);
    return c;
}

