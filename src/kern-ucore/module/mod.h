#ifndef __KERN_MODULE_MOD_H__
#define  __KERN_MODULE_MOD_H__

#include <stdio.h>

struct elf_mod_info_s {
     uintptr_t image;
     uint32_t  image_size;
     
     uintptr_t ptr;
     uintptr_t common_ptr;
     uint32_t  common_size;
     uintptr_t load_ptr;
     uintptr_t unload_ptr;
};

void mod_init();
int elf_mod_load(uintptr_t image, uint32_t image_size, struct elf_mod_info_s * info);

#endif
