#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <assert.h> // RB delete

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "elf64.h"

//     // child = runTarget(argv);
//     // runDebug(child);

//     return 0;
// }

// // pid_t runTarget(const char** args){
// //     pid_t pid = fork();
// //     if(pid == 0){ // child
// //         if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0){
// //             perror("ptrace");
// //             exit(1);
// //         }
// //         execv(argv[2], &argv[2]);
// //     } else if(pid > 0) { // parent
// //         return pid;
// //     } else { // fork err
// //         perror("fork");
// //         exit(1);
// //     }
// // }

int main(int argc, char* argv[]){
    
    FILE* f = NULL;
    Elf64_Ehdr elf_hdr;

    if(argc != 3) {
        printf("wrong usage\n");
        exit(1);
    }

    if((f = fopen(argv[2], "r")) == NULL) {
        perror("fopen");
        exit(1);
    }

    fread(&elf_hdr, 1, sizeof(Elf64_Ehdr), f);
    Elf64_Shdr* shdrs = malloc(elf_hdr.e_shnum * elf_hdr.e_shentsize);

    fseek(f, 0L, SEEK_SET);
    fseek(f, elf_hdr.e_shoff, SEEK_SET);
    fread(shdrs, 1, elf_hdr.e_shnum * elf_hdr.e_shentsize, f);

    // find symtable
    unsigned int symtables = 0, strtab_i = 0;
    size_t symtables_total = 0;
    for(int i = 0; i<elf_hdr.e_shnum; i++){
        if(shdrs[i].sh_type == 0x2){
            symtables++;
            symtables_total += shdrs[i].sh_size;
        } else if (shdrs[i].sh_type == 0x3 && i != elf_hdr.e_shstrndx){
            strtab_i = i;
        }
    }

    // get symbol names
    int ctr = 0;
    Elf64_Sym* sym_names = malloc(symtables_total);
    for(int i=0; i<elf_hdr.e_shnum; i++){
        if(shdrs[i].sh_type == 0x2){
            fseek(f, 0L, SEEK_SET);
            fseek(f, shdrs[i].sh_offset, SEEK_SET);
            fread(&(sym_names[ctr]), 1, shdrs[i].sh_size, f);
            ctr++;
        }
    }

    // read all strings from .strtab section
    char* all_strs = malloc(shdrs[strtab_i].sh_size);
    fread(all_strs, shdrs[strtab_i].sh_size, 1, f);

    // find required func in symbol table
    int func_i = -1;
    int sym_num = symtables_total / sizeof(Elf64_Sym);
    for(int i=0; i<sym_num; i++){
        if(strcmp(&all_strs[sym_names[i].st_name], argv[1]) == 0){
            func_i = i;
            break;
        }
    }
    free(all_strs);

    // print error if not found
    if(func_i == -1){
        printf("PRF:: not found!\n");
        fclose(f);
        return 0;
    }

    // print error if not global
    if(ELF64_ST_BIND(sym_names[func_i].st_info) == 0){
        printf("PRF:: local found!\n");
        fclose(f);
        return 0;
    }

    size_t f_add = sym_names[func_i].st_value;

    // child = runTarget(argv);
    // runDebug(child);


    fclose(f);
    return 0;
}

void runDebug(pid_t child){

}

pid_t runTarget(const char** argv){
    pid_t pid = fork();
    if(pid == 0){ // child
        if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0){
            perror("ptrace");
            exit(1);
        }
        execv(argv[2], &argv[2]);
    } else if(pid > 0) { // parent
        return pid;
    } else { // fork err
        perror("fork");
        exit(1);
    }
}