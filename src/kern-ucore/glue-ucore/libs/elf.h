#ifndef __LIBS_ELF_H__
#define __LIBS_ELF_H__

#include <types.h>

#define ELF_MAGIC   0x464C457FU         // "\x7FELF" in little endian

/* file header */
struct elfhdr {
    uint32_t e_magic;     // must equal ELF_MAGIC
    uint8_t e_elf[12];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
}__attribute__((packed));

/* program section header */
struct proghdr {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_va;
    uint64_t p_pa;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
};

/* section header */
struct secthdr {
     uint32_t sh_name;
     uint32_t sh_type;
     uint64_t sh_flags;
     uint64_t sh_addr;
     uint64_t sh_offset;
     uint64_t sh_size;
     uint32_t sh_link;
     uint32_t sh_info;
     uint64_t sh_addralign;
     uint64_t sh_entsize;
}__attribute__((packed));

struct reloc_s {
    uint64_t rl_offset;         /* Location at which to apply the action */
    uint64_t rl_info;           /* index and type of relocation */
} __attribute__((packed));

struct reloc_a_s {
    uint64_t rl_offset;         /* Location at which to apply the action */
    uint64_t rl_info;           /* index and type of relocation */
    int64_t rl_addend;          /* Constant addend used to compute value */
} __attribute__((packed));

struct symtab_s {
    uint32_t sym_name;
    uint8_t  sym_info;
    uint8_t  sym_other;
    uint16_t sym_shndx;
    uint64_t sym_address;
    uint64_t sym_size;
} __attribute__((packed));

#define SH_TYPE_SYMTAB 2
#define SH_TYPE_STRTAB 3
#define SH_TYPE_NOBITS 8
#define SH_TYPE_RELOC  9

/* values for Proghdr::p_type */
#define ELF_PT_LOAD                     1

/* flag bits for Proghdr::p_flags */
#define ELF_PF_X                        1
#define ELF_PF_W                        2
#define ELF_PF_R                        4

#endif /* !__LIBS_ELF_H__ */

