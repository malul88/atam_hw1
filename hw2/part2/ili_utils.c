#include <asm/desc.h>
#include <linux/mm.h>


void my_store_idt(struct desc_ptr *idtr) { 
    asm volatile ("sidt %0" : "=m"(*idtr));
}

void my_load_idt(struct desc_ptr *idtr) { 
    asm ("lidt %0" :: "m"(*idtr));
}

void my_set_gate_offset(gate_desc *gate, unsigned long addr) {
    unsigned short max_short = 65535;
    unsigned int max_int = 4294967295;

    unsigned long needed_bits = addr;
	gate->offset_low = (unsigned short) (needed_bits & max_short);

    needed_bits = needed_bits >> 16;
	gate->offset_middle = (unsigned short) (needed_bits & max_short);

    needed_bits = needed_bits >> 16;
	gate->offset_high = (unsigned int) (needed_bits & max_int);
}

unsigned long my_get_gate_offset(gate_desc *gate) {
	unsigned long to_ret = 0;
    unsigned long to_ret_low = (unsigned long) gate->offset_low;
    unsigned long to_ret_mid = (unsigned long) gate->offset_middle;
    unsigned long to_ret_high = (unsigned long) gate->offset_high;

    to_ret = to_ret_low | (to_ret_mid << 16) | (to_ret_high << 32);
	
    return to_ret;
}
