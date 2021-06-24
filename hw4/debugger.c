#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "elf64.h"

int main(int argc, char* argv[]){

    FILE* ElfFile = NULL;
    char* SectNames = NULL;
    Elf64_Ehdr elfHdr;
    Elf64_Shdr sectHdr;
    uint32_t idx;

    if(argc != 2) {
        printf("usage: %s <ELF_FILE>\n", argv[0]);
        exit(1);
    }

    if((ElfFile = fopen(argv[1], "r")) == NULL) {
        perror("[E] Error opening file:");
        exit(1);
    }

    // read ELF header, first thing in the file
    fread(&elfHdr, 1, sizeof(Elf64_Ehdr), ElfFile);

    // read section name string table
    // first, read its header. 
    /* 
    e_shoff         This member holds the section header table's file offset
                    in bytes.  If the file has no section header table, this
                    member holds zero.

    e_shstrndx      This member holds the section header table index of the
                entry associated with the section name string table.  If
            the file has no section name string table, this member
            holds the value SHN_UNDEF.

            If the index of section name string table section is
            larger than or equal to SHN_LORESERVE (0xff00), this
            member holds SHN_XINDEX (0xffff) and the real index of
            the section name string table section is held in the
            sh_link member of the initial entry in section header
            table.  Otherwise, the sh_link member of the initial
            entry in section header table contains the value zero.

            SHN_UNDEF     This value marks an undefined, missing,
                            irrelevant, or otherwise meaningless
                            section reference.  For example, a symbol
                            "defined" relative to section number
                            SHN_UNDEF is an undefined symbol.

            SHN_LORESERVE This value specifies the lower bound of the
                            range of reserved indices.

            SHN_LOPROC    Values greater than or equal to SHN_HIPROC
                            are reserved for processor-specific
                            semantics.

            SHN_HIPROC    Values less than or equal to SHN_LOPROC are
                            reserved for processor-specific semantics.

            SHN_ABS       This value specifies absolute values for
                            the corresponding reference.  For example,
                            symbols defined relative to section number
                            SHN_ABS have absolute values and are not
                            affected by relocation.

            SHN_COMMON    Symbols defined relative to this section
                            are common symbols, such as Fortran COMMON
                            or unallocated C external variables.

            SHN_HIRESERVE This value specifies the upper bound of the
                            range of reserved indices between
                            SHN_LORESERVE and SHN_HIRESERVE, inclusive;
                            the values do not reference the section
                            header table.  That is, the section header
                            table does not contain entries for the
                            reserved indices.
    */
    fseek(ElfFile, elfHdr.e_shoff + elfHdr.e_shstrndx * sizeof(sectHdr), SEEK_SET);
    fread(&sectHdr, 1, sizeof(sectHdr), ElfFile);
    /*
    sh_size       This member holds the section's size in bytes.  Unless the
                    section type is SHT_NOBITS, the section occupies sh_size
                    bytes in the file.  A section of type SHT_NOBITS may have a
                    nonzero size, but it occupies no space in the file.

    sh_offset     This member's value holds the byte offset from the
                    beginning of the file to the first byte in the section.
                    One section type, SHT_NOBITS, occupies no space in the
                    file, and its sh_offset member locates the conceptual
                    placement in the file.

    e_shnum       This member holds the number of entries in the section
                    header table.  Thus the product of e_shentsize and
                    e_shnum gives the section header table's size in bytes.
                    If a file has no section header table, e_shnum holds the
                    value of zero.

                    If the number of entries in the section header table is
                    larger than or equal to SHN_LORESERVE (0xff00), e_shnum
                    holds the value zero and the real number of entries in
                    the section header table is held in the sh_size member of
                    the initial entry in section header table.  Otherwise,
                    the sh_size member of the initial entry in the section
                    header table holds the value zero.   

    sh_name       This member specifies the name of the section.  Its value
                    is an index into the section header string table section,
                    giving the location of a null-terminated string.
    */
    // next, read the section, string data
    // printf("sh_size = %llu\n", sectHdr.sh_size);
    SectNames = malloc(sectHdr.sh_size);
    fseek(ElfFile, sectHdr.sh_offset, SEEK_SET);
    fread(SectNames, 1, sectHdr.sh_size, ElfFile);

    // read all section headers
    for (idx = 0; idx < elfHdr.e_shnum; idx++)
    {
        const char* name = "";

        fseek(ElfFile, elfHdr.e_shoff + idx * sizeof(sectHdr), SEEK_SET);
        fread(&sectHdr, 1, sizeof(sectHdr), ElfFile);

        // print section name
        if (sectHdr.sh_name);
        name = SectNames + sectHdr.sh_name;
        printf("%2u %s\n", idx, name);
    }


    // pid_t child;
    // FILE* file;

    

    // file = fopen(argv[2], "rb");
    // if(file == NULL){
    //     perror("fopen");
    //     exit(1);
    // }

    // char buf[500];
    // fread(buf, 499, 1, file);
    // for(int i=0; i<500; i++){
    //     printf("%c", buf[i]);
    // }
    // printf("\n");

    // fclose(file);


    // child = runTarget(argv);
    // runDebug(child);

    return 0;
}

// pid_t runTarget(const char** args){
//     pid_t pid = fork();
//     if(pid == 0){ // child
//         if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0){
//             perror("ptrace");
//             exit(1);
//         }
//         execv(argv[2], &argv[2]);
//     } else if(pid > 0) { // parent
//         return pid;
//     } else { // fork err
//         perror("fork");
//         exit(1);
//     }
// }

void runDebug(pid_t child){

}