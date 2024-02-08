#include <stdint.h>
#include <stddef.h>

#include "lib/std.h"

//#define print(x); kprint_yx(x, POS++, 0, D);
//#define cprint(x, y); kprint_yx(x, POS++, 0, y);

// [0x000000000602] 0008:0000000000000602 (unk. ctxt): add byte ptr ds:[rax], al ; 0000

struct idt_entry {
	uint16_t base_lo;
	uint16_t sel;
	uint8_t ist;
	uint8_t flags;
	uint16_t base_mid;
	uint32_t base_hi;
	uint32_t reserved;
} __attribute__((packed));

struct idt_ptr {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;

void handle_interrupt(void);
void cause_div_problems(void);
void idt_install(void);
//void idt_load(struct idt_ptr* idtp);
void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags);

void main(void){


	//int POS = 1; // keep track of lines printed
	char cpuname[16];
	unsigned int lm_bit;

	screen_clear();
	kprint("screen cleared..\n");
	print("testing stack... seems to work             \n");
	char test[] = "testing data section, seems to work    \n";
	print(test);
	screen_paint(GREEN);

	// check some assumtions about the cpu
	// this assumes CPUID is avilable ont he current platform
	//print("lets read CPUID..");
	cpu_id(cpuname);
	cprint(cpuname, LIGHTBLUE);

	print("lets check CPUID for long mode support");
	lm_bit = cpu_lm_bit();
	if (lm_bit & (1 << 29)) {
		cprint("cool: long_mode supported", BLUE);
		// ok, we can procede
	} else {
		cprint("non_cool: long mode NOT supported", RED);
		// we should die..
	}

	cprint("apparently we are in long mode.... AND PAGING", YELLOW);

	init_heap();

	void * foo = malloc(1);
	void * fooo = malloc(15);
	void * foooo = malloc(16);
	void * abc = malloc(27);

	int iiiii  = 0;

	//idt_install();

	__asm__ volatile ("sti"); // set the interrupt flag

	 //__asm__ volatile ("cli; hlt");

	handle_interrupt();
	//
	cause_div_problems();
	//
	//extern void init_idt();
	//cause_page_problems();
	for(;;){
		//kprint_yx(".", POS++, 0, D);
//iiiii --;
	}
}


void idt_load(struct idt_ptr idtp){
	__asm__ volatile("lidt (%0)"   : : "m"(idtp) : "memory");
}


void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
	idt[num].base_lo = (base & 0xFFFF);
	idt[num].base_mid = (base >> 16) & 0xFFFF;
	idt[num].base_hi = (base >> 32) & 0xFFFFFFFF;
	idt[num].sel = sel;
	idt[num].ist = 0;
	idt[num].flags = flags;
	idt[num].reserved = 0;
}

void idt_install(void){
	idtp.limit = (sizeof ( struct idt_entry ) * 256 ) -1;
	idtp.base = (uint64_t) &idt;
	for (int i = 0; i < 256; i++) {
		//idt_set_gate(i, /* ISR address */, /* segment */, /* flags */);
		idt_set_gate(i, 0x1111111, 0x08, 0x8e );
	}

	//idt_load(&idtp);
	idt_load(idtp);
}



// handle_interrupt prints a generic message to the screen and does not return.
void handle_interrupt(void) {
    // print red on black
    //char color_code = vga_color_code(VGA_COLOR_RED, VGA_COLOR_BLACK);
    //vga_print(color_code, "\nInterrupt handled!");
	print("INTERUP KINDA HANDLED");
}

void cause_div_problems(void){
	int b = 0;
	int a = 9 / b;
}

//void cause_page_problems(void){
//	char *page_fault = (char *) 0x400000;
//	// Assigning a value to the memory address causes a page fault.
//	*page_fault = 42;
//}

