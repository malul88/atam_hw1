#include <asm/desc.h>

// Do we need this? RB
struct idt_t {
    void *base;
    unsigned short length;
};

void my_store_idt(struct desc_ptr *idtr) {
    // read idtr into memory
    struct idt_t idt;
    asm volatile ("sidt %0" : "=m"(*idt));

    // copy idtr
    idtr->address = __get_free_page(GFP_KERNEL);
    idtr->size = idt.length;
    memcpy(idtr->address, idt.base, idt.length);
}

void my_load_idt(struct desc_ptr *idtr) { // might need to use desc_ptr struct instead of idt_t 
    struct idt_t idt;
    idt.base = idtr->address;
    idt.size = idtr->length;
    asm volatile ("lidt %0" :: "m"(idt));
}

void my_set_gate_offset(gate_desc *gate, unsigned long addr) {
    gate->offset_low = 0;
    for(int i=0; i<16; i++){
        int bit = addr%2;
        bit = bit << i;
        gate->offset_low += bit;
        addr >>= 1;
    }

    gate->offset_middle = 0;
    for(int i=0; i<16; i++){
        int bit = addr%2;
        bit = bit << i;
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
