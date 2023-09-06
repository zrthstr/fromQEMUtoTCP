#include <stdint.h>
#include <stddef.h>

#include "lib/std.h"

//#define print(x); kprint_yx(x, POS++, 0, D);
//#define cprint(x, y); kprint_yx(x, POS++, 0, y);


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

	//handle_interrupt();
	//
	//cause_div_problems();
	extern void asm_from_kernel();
	asm_from_kernel();
	//cause_page_problems();
	for(;;){
		//kprint_yx(".", POS++, 0, D);
		iiiii --;
	}
}

//interrupt.c
//#include "vga.h"

// handle_interrupt prints a generic message to the screen and does not return.
void handle_interrupt() {
    // print red on black
    //char color_code = vga_color_code(VGA_COLOR_RED, VGA_COLOR_BLACK);
    //vga_print(color_code, "\nInterrupt handled!");
	print("INTERUP KINDA HANDLED");
    while(1) {};
}

void cause_div_problems(){
	int b = 0;
	int a = 9 / b;
}
void cause_page_problems(){
	char *page_fault = (char *) 0x400000;
	// Assigning a value to the memory address causes a page fault.
	*page_fault = 42;
}
