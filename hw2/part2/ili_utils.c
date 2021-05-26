#include <asm/desc.h>
#include <linux/mm.h>

// Do we need this? RB
struct idt_t {
    void *base;
    unsigned short length;
};

// void my_store_idt(struct desc_ptr *idtr) {
//     // read idtr into memory
//     struct idt_t idt;
//     asm volatile ("sidt %0" : "=m"(idt));

//     // copy idtr
//     idtr->address = __get_free_page(GFP_KERNEL);
//     idtr->size = idt.length;
//     memcpy((void *)idtr->address, idt.base, idt.length);
// }

void my_store_idt(struct desc_ptr *idtr) {
    // read idtr into memory
    struct desc_ptr idt;
    asm volatile ("sidt %0" : "=m"(idt));

    // copy idtr
    idtr->address = __get_free_page(GFP_KERNEL);
    idtr->size = idt.size;
    memcpy((void *)idtr->address, (void *)idt.address, idt.size);
}

// void my_load_idt(struct desc_ptr *idtr) { // might need to use desc_ptr struct instead of idt_t 
//     struct idt_t idt;
//     idt.base = (void *)idtr->address;
//     idt.length = idtr->size;
//     asm volatile ("lidt %0" :: "m"(idt));
// }

void my_load_idt(struct desc_ptr *idtr) { // might need to use desc_ptr struct instead of idt_t 
    struct desc_ptr idt;
    idt.address = idtr->address;
    idt.size = idtr->size;
    asm volatile ("lidt %0" :: "m"(idt));
}

void my_set_gate_offset(gate_desc *gate, unsigned long addr) {
    int i = 0, j = 0, bit;
    gate->offset_low = 0;
    for(; i<16; i++){
        bit = addr%2;
        bit = bit << i;
        gate->offset_low += bit;
        addr >>= 1;
    }

    gate->offset_middle = 0;
    for(; j<16; j++){
        bit = addr%2;
        bit = bit << j;
        gate->offset_middle += bit;
        addr >>= 1;
    }

    gate->offset_high = addr;
}

unsigned long my_get_gate_offset(gate_desc *gate) {
    //gate = &idt_entry for invalid opcode
    unsigned long offset = 0;
    offset += gate->offset_high;
    offset = offset << 16;
    offset += gate->offset_middle;
    offset = offset << 16;
    offset += gate->offset_low;

    return offset;
}
