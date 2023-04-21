#include <stdint.h>
#include <stddef.h>

#include "lib/std.h"

//#define print(x); kprint_yx(x, POS++, 0, D);
//#define cprint(x, y); kprint_yx(x, POS++, 0, y);


void main(void){

  /* lets cause a fault */
  /*
	int FLAG = 0;
	if (FLAG == 1){
		int b = 0;
		int a = 9 / b;
	}
	*/

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
	for(;;){
		//kprint_yx(".", POS++, 0, D);
		iiiii --;
	}
}

