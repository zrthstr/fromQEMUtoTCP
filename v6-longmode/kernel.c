#include <stdint.h>
#include <stddef.h>


#define BLACK 0x00
#define BLUE 0x01
#define RED 0x4
#define DARKGRAY 0x07
#define LIGHTBLUE 0x09
#define GREEN 0x0A
#define LIGHTCYAN 0x0B
#define VGA_BUFF_START 0xB8000
#define VGA_BUFF_END 0xB8FA0
#define YELLOW 0x0E

#define D DARKGRAY

#define print(x); kprint_yx(x, POS++, 0, D);
#define cprint(x, y); kprint_yx(x, POS++, 0, y);


int kprint(const char *string);
int kprint_yx(const char *string, int y, int x, char color);
void screen_clear(void);
void screen_paint(int);
//void cpuid(void);
void cpu_id(const char *cpuname);
unsigned int cpu_lm_bit(void);
//int cpu_chk_interups_64(void);
uint64_t get_cr0_64(void);
void int_to_bin_str(int, char *);
void int_to_hex_str(int, char *);


void main(void){


  /* lets cause a fault */
  /*
	int FLAG = 0;
	if (FLAG == 1){
		int b = 0;
		int a = 9 / b;
	}
	*/
	int POS = 1; // keep track of lines printed
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

	
	/*
	print("check if interups are enabled..  \n");
	int interupts_enabled = cpu_chk_interups_64();
	if (interupts_enabled){
		print("interupts enbaled");
	} else {
		print("interupts DISabled");
	}
	*/

	// uint64_t cr0 = get_cr0_64();
	// char cr0_str[33];
	// int_to_bin_str(cr0, cr0_str);

	// print("cr0 -> ");
	// print(cr0_str);


	cprint("apparently we are in long mode.... AND PAGING", YELLOW);

	int iiiii  = 0;
	for(;;){
		//kprint_yx(".", POS++, 0, D);
		iiiii --;
	}
}

/*
int get_cr0(void){
	uint64_t cr0;
	__asm__ volatile(
		"mov cr0, ax":"=r"(cr0));
	return cr0;
}*/

uint64_t get_cr0_64(void) {
    uint64_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r" (cr0));
    return cr0;
}


void int_to_bin_str(int n, char * str){
	for (int i = 0; i < 32; i++){
		str[i] = ((n >> (31 - i)) & 1 ) + '0';
	}
}

void int_to_hex_str(int n, char * str){
	char map[] = "0123456789abcdef";
	int32_t mask = 0b1111;
	int i = 0;
	for (; i < 8; i++){
		int32_t ix = ( n >> ((7 - i) * 4) ) & mask;
		str[i] = map[ix];
	}
	str[i] = '\0'; 
}

/*
int cpu_chk_interups(void){
	unsigned int flags;
	__asm__ volatile ("pushfq\n"
		"pop eax"
		:"=a"(flags));
	return flags & (1 << 9);
}*/

/*
int cpu_chk_interups_64(void) {
    unsigned int flags;
    __asm__ volatile ("pushfq\n" "pop rax\n"
                      "mov %%rax, %0\n" : "=r" (flags));
    return flags & (1 << 9);
}*/

void cpu_id(const char * cpuname) {
	// get the cpu vendor given name
	//cpuid_string(0, (int*)(cpuname));

	int code = 0; /// magic number, CPU function
	int * where = (int *) cpuname;

	__asm__ volatile (
		"cpuid"
		:"=a"(*where),"=b"(*(where+0)),"=d"(*(where+1)),"=c"(*(where+2))
		:"a"(code));
}

unsigned int cpu_lm_bit(void){
	// in asm we would want to do sth like this
	//    mov eax, 0x80000001    ; Set the A-register to 0x80000001.
	//    cpuid                  ; CPU identification.
	//    test edx, 1 << 29      ; Test if the LM-bit, which is bit 29, is set in the D-register.
	//    jz .NoLongMode         ; They aren't, there is no long mode.

	unsigned int mode;
	__asm__ volatile(
		"mov eax, 0x80000001 \n"
		"cpuid "
		:"=d"(mode));
	return mode;
}


int kprint(const char *string){
	int x = 0;
	int y = 0;
	int n = kprint_yx(string, y, x, DARKGRAY);
	return n;
}

int kprint_yx(const char *string, int y, int x, char color){
	/*  x => start char; y => start line */

	int n = 0;
	//char color = DARKGRAY;
	int offset = (x + y * 80) * 2;
	volatile char *video = (volatile char*)0xB8000 + offset;
	while ( *string != 0){
		n++;
		*video++ = *string++;
		*video++ = color;
	}
	return n;
}

void screen_clear(void){
	volatile char *video = (volatile char*)0xB8000;
	while ( video <= VGA_BUFF_END){
		*video++ = 0;
		*video++ = 0;
	}
}

void screen_paint(int color){
	volatile char *video = (volatile char*)0xB8000;
	while ( video <= VGA_BUFF_END){
		*video++;
		*video++ = color;
	}
}

