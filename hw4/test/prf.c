#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// #include <assert.h> // RB delete

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include "elf64.h"

void perrAndExit(const char* str){
    perror(str);
    exit(1);
}

void Ptrace(enum __ptrace_request r, pid_t pid, void* addr, void* data){
    if(ptrace(r, pid, addr, data) == -1){
        perrAndExit("ptrace");
    }
}

long setBreak(pid_t child, size_t f_add, int* wait_status, struct user_regs_struct* regs){
    long data = ptrace(PTRACE_PEEKTEXT, child, (void*)f_add, NULL);
    if(data == -1){
        perrAndExit("ptrace");
    }

    // printf("DBG: Added breakpoint at address: 0x%lx\nDBG: Data in this address: 0x%lx\n", f_add, data); // for debug RB

    long data_trap = (data & 0xFFFFFFFFFFFFFF00) | 0xCC;
    Ptrace(PTRACE_POKETEXT, child, (void*)f_add, (void*)data_trap);
    return data;
}

void remBreak(pid_t child, size_t f_add, int* wait_status, struct user_regs_struct* regs, long orig_data){
    Ptrace(PTRACE_GETREGS, child, 0, regs);
    // printf("DBG: Child stopped at RIP = 0x%x\n", regs->rip);

    /* Remove the breakpoint by restoring the previous data */
    Ptrace(PTRACE_POKETEXT, child, (void*)f_add, (void*)orig_data);
    regs->rip -= 1;
    Ptrace(PTRACE_SETREGS, child, 0, regs);
}

pid_t runTarget(char** argv){
    pid_t pid = fork();
    if(pid == 0){ // child
        if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0){
            perrAndExit("ptrace");
        }
        if(execv(argv[2], &argv[2]) < 0){
            perrAndExit("execv");
        }
    } else if(pid > 0) { // parent
        return pid;
    } else { // fork err
        perrAndExit("fork");
    }
}

void runDebug(pid_t child, size_t f_add){
    int wait_status, sysctr = 0;
    long data;
    struct user_regs_struct regs;
    size_t old_rsp, new_rsp;

    if(wait(&wait_status) < 0){
        perrAndExit("wait");
    }

    while(!WIFEXITED(wait_status)){ // while process is still running

        // set breakpoint
        data = setBreak(child, f_add, &wait_status, &regs);

        /* The child can continue running now */
        Ptrace(PTRACE_CONT, child, 0, 0);
        if(wait(&wait_status) < 0){
            perrAndExit("wait");
        }

        // check if process finished
        if(WIFEXITED(wait_status)){
            break;
        }    

        // remove breakpoint
        remBreak(child, f_add, &wait_status, &regs, data);
        Ptrace(PTRACE_GETREGS, child, NULL, &regs);
        old_rsp = regs.rsp;


        // check all instructions in func
        while (WIFSTOPPED(wait_status)) { 
            
            Ptrace(PTRACE_GETREGS, child, NULL, &regs);
            unsigned long instr = ptrace(PTRACE_PEEKTEXT, child, regs.rip, NULL);
            unsigned short last_two = instr & 0x000000000000FFFF;
            size_t old_rip = regs.rip;


            // check if program returned from func
            if(regs.rsp == old_rsp + 8){ 
                // printf("DBG: returned from func\n"); // remove later RB
                break;
            }

            /* Make the child execute another instruction */
            Ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);

            /* Wait for child to stop on its next instruction */
            wait(&wait_status);

            // check if instr is syscall
            if(last_two == 0x050F){ 
                sysctr++;
                // printf("DBG: syscalls found inside func: %d\n", sysctr); // remove later RB
                Ptrace(PTRACE_GETREGS, child, NULL, &regs);
                if((long int) regs.rax < 0){
                    printf("PRF:: syscall in %x returned with %ld\n", (unsigned int) old_rip, (long int) regs.rax);
                    // printf("PRF:: syscall\n");

                }
            }
        }
    }

    if (WIFEXITED(wait_status)) {
        // printf("DBG: Child exited\n"); // remove later RB
    } else {
        // printf("DBG: Unexpected signal\n"); // remove later RB
    }
}


int main(int argc, char** argv){
    
    FILE* f = NULL;
    Elf64_Ehdr elf_hdr;

    if((f = fopen(argv[2], "r")) == NULL) {
        perrAndExit("fopen");
    }

    fread(&elf_hdr, 1, sizeof(Elf64_Ehdr), f);
    Elf64_Shdr* shdrs = malloc(elf_hdr.e_shnum * elf_hdr.e_shentsize);

    fseek(f, 0L, SEEK_SET);
    fseek(f, elf_hdr.e_shoff, SEEK_SET);
    fread(shdrs, 1, elf_hdr.e_shnum * elf_hdr.e_shentsize, f);

    // find symtable
    unsigned int symtables = 0;
    size_t symtables_total = 0;
    for(int i = 0; i<elf_hdr.e_shnum; i++){
        if(shdrs[i].sh_type == 0x2){
            symtables++;
            symtables_total += shdrs[i].sh_size;
        }
    }

    // get symbol names
    int ctr = 0, last_j = 0;
    Elf64_Sym* sym_names = malloc(symtables_total);
    Elf64_Word* sym_names_links = malloc((symtables_total / sizeof(Elf64_Sym)) * sizeof(Elf64_Word));
    for(int i=0; i<elf_hdr.e_shnum; i++){
        if(shdrs[i].sh_type == 0x2){
            fseek(f, 0L, SEEK_SET);
            fseek(f, shdrs[i].sh_offset, SEEK_SET);
            fread(&(sym_names[ctr]), 1, shdrs[i].sh_size, f);

            // fill related links of symbols
            for(int j=last_j; j<last_j+(shdrs[i].sh_size/sizeof(Elf64_Sym)); j++){
                sym_names_links[j] = shdrs[i].sh_link;
            }
            last_j += shdrs[i].sh_size / sizeof(Elf64_Sym);
            
            ctr++;
        }
    }

    // read all strings from .strtab section
    Elf64_Word curr_link = sym_names_links[0];
    char* all_strs = malloc(shdrs[curr_link].sh_size);
    fseek(f, 0L, SEEK_SET);
    fseek(f, shdrs[curr_link].sh_offset, SEEK_SET);
    fread(all_strs, shdrs[curr_link].sh_size, 1, f);

    // find required func in symbol table
    int func_i = -1;
    int sym_num = symtables_total / sizeof(Elf64_Sym);
    for(int i=0; i<sym_num; i++){
        Elf64_Sym curr_sym = sym_names[i];

        if(sym_names_links[i] != curr_link){
            curr_link = sym_names_links[i];
            free(all_strs);
            all_strs = malloc(shdrs[curr_link].sh_size);
            fseek(f, 0L, SEEK_SET);
            fseek(f, shdrs[curr_link].sh_offset, SEEK_SET);
            fread(all_strs, shdrs[curr_link].sh_size, 1, f);
        }

        if(strcmp(&all_strs[sym_names[i].st_name], argv[1]) == 0){
            func_i = i;
            break;
        }
    }
    free(all_strs);

    // print error if not found
    if(func_i == -1){
        printf("PRF:: not found!\n");
        free(shdrs);
        free(sym_names);
        free(sym_names_links);
        fclose(f);
        return 0;
    }

    // print error if not global
    if(ELF64_ST_BIND(sym_names[func_i].st_info) == 0){
        printf("PRF:: local found!\n");
        free(shdrs);
        free(sym_names);
        free(sym_names_links);
        fclose(f);
        return 0;
    }

    size_t f_add = sym_names[func_i].st_value;

    pid_t child = runTarget(argv);
    runDebug(child, f_add);

    free(shdrs);
    free(sym_names);
    free(sym_names_links);
    fclose(f);
    return 0;
}
