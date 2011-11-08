#ifndef __KERN_MODULE_ELF_H__
#define __KERN_MODULE_ELF_H__

#include <types.h>

#define ELF_MAGIC 0x464C457FU   /* "\x7FELF" in little endian */

struct elf_s {
	 uint32_t e_magic;       // must equal ELF_MAGIC
	 uint8_t  e_elf[12];
	 uint16_t e_type;
	 uint16_t e_machine;
	 uint32_t e_version;
	 uint32_t e_entry;
	 uint32_t e_phoff;
	 uint32_t e_shoff;
	 uint32_t e_flags;
	 uint16_t e_ehsize;
	 uint16_t e_phentsize;
	 uint16_t e_phnum;
	 uint16_t e_shentsize;
	 uint16_t e_shnum;
	 uint16_t e_shstrndx;
};

struct proghdr_s {
	 uint32_t p_type;
	 uint32_t p_offset;
	 uint32_t p_va;
	 uint32_t p_pa;
	 uint32_t p_filesz;
	 uint32_t p_memsz;
	 uint32_t p_flags;
	 uint32_t p_align;
};

struct secthdr_s {
	 uint32_t sh_name;
	 uint32_t sh_type;
	 uint32_t sh_flags;
	 uint32_t sh_addr;
	 uint32_t sh_offset;
	 uint32_t sh_size;
	 uint32_t sh_link;
	 uint32_t sh_info;
	 uint32_t sh_addralign;
	 uint32_t sh_entsize;
};

struct reloc_s
{
	 int      rl_offset;
	 uint32_t rl_info;
} __attribute__((packed));

struct reloc_a_s
{
	 int      rl_offset;
	 uint32_t rl_info;
	 uint32_t rl_addend;
} __attribute__((packed));

struct symtab_s
{
	 uint32_t sym_name;
	 uint32_t sym_address;
	 uint32_t sym_size;
	 uint8_t  sym_info;
	 uint8_t  sym_other;
	 uint16_t sym_shndx;
} __attribute__((packed));

// Values for Proghdr::p_type
#define ELF_PROG_LOAD           1

// Flag bits for Proghdr::p_flags
#define ELF_PROG_FLAG_EXEC      1
#define ELF_PROG_FLAG_WRITE     2
#define ELF_PROG_FLAG_READ      4

// Values for Secthdr::sh_type
#define ELF_SHT_NULL            0
#define ELF_SHT_PROGBITS        1
#define ELF_SHT_SYMTAB          2
#define ELF_SHT_STRTAB          3

// Values for Secthdr::sh_name
#define ELF_SHN_UNDEF           0

#endif /* _KERNEL_ELF_H_ */
