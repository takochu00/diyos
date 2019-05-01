#include <typedefs.h>
#include <elf.h>
#include <lib.h>

typedef struct ELF_HEADER {
    struct {
        uint8_t magic[4];//magic number
        uint8_t class;
        uint8_t format;//endian
        uint8_t version;
        uint8_t abi;
        uint8_t abi_version;
        uint8_t reserved[7];
    } id;
    uint16_t type;//file type
    uint16_t arch;//cpu
    uint32_t version;
    uint32_t entry_point;
    uint32_t program_header_offset;
    uint32_t section_header_offset;
    uint32_t flags;
    uint16_t header_size;
    uint16_t program_header_size;
    uint16_t program_header_num;
    uint16_t section_header_size;
    uint16_t section_header_num;
    uint16_t section_name_index;
} ELF_HEADER;

typedef struct ELF_PROGRAM_HEADER {
    uint32_t type;
    uint32_t offset;
    uint32_t virtual_addr;
    uint32_t physical_addr;
    uint32_t file_size;
    uint32_t memory_size;
    uint32_t flags;
    uint32_t align;
} ELF_PROGRAM_HEADER;

#define ELF_MAGIC_NUMBER "\x7f" "ELF"
#define ELF_HEADER_ID_CLASS_ELF32 1
#define ELF_HEADER_ID_FORMAT_BIG_ENDIAN 2
#define ELF_HEADER_ID_VERSION_V1 1
#define ELF_HEADER_TYPE_EXE 2
#define ELF_HEADER_VERSION_V1 1
#define ELF_HEADER_ARCH_H8300 46
#define ELF_HEADER_ARCH_H8300H 47

static int elf_check(ELF_HEADER *header){
    if(memcmp(header->id.magic, ELF_MAGIC_NUMBER, 4) != 0) return -1;
    if(header->id.class != ELF_HEADER_ID_CLASS_ELF32) return -1;
    if(header->id.format != ELF_HEADER_ID_FORMAT_BIG_ENDIAN) return -1;
    if(header->id.version != ELF_HEADER_ID_VERSION_V1) return -1;
    if(header->type != ELF_HEADER_TYPE_EXE) return -1;
    if(header->version != ELF_HEADER_VERSION_V1) return -1;
    if((header->arch != ELF_HEADER_ARCH_H8300) && (header->arch != ELF_HEADER_ARCH_H8300H)) return -1;
    return 0;
}

static int elf_load_program(ELF_HEADER *header){
    int i;
    for(i = 0; i < header->program_header_num; ++i){
        ELF_PROGRAM_HEADER *phdr = (ELF_PROGRAM_HEADER *)((uint8_t *)header + header->program_header_offset +
                                                          header->program_header_size * i);
        //type == 1 means loadable
        if(phdr->type != 1) continue;
        //check loadable file. So far, value check only.
        putxval(phdr->offset, 6); putchar(' ');
        putxval(phdr->virtual_addr, 8); putchar(' ');
        putxval(phdr->physical_addr, 8); putchar(' ');
        putxval(phdr->file_size, 5); putchar(' ');
        putxval(phdr->memory_size, 5); putchar(' ');
        putxval(phdr->flags, 2); putchar(' ');
        putxval(phdr->align, 2); putchar('\n');
    }
    return 0;
}

int elf_load(uint8_t *buf){
    ELF_HEADER *header = (ELF_HEADER *)buf;
    if(elf_check(header) < 0){
        printf("ELF HEADER Check failed\n");
        return -1;
    }
    if(elf_load_program(header) < 0){
        printf("ELF LOAD PROGRAM failed\n");
        return -1;
    }
    return 0;
}
