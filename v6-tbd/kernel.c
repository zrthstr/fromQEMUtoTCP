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
void set_up_paging(void);
//void cpuid(void);
void cpu_id(const char *cpuname);
unsigned int cpu_lm_bit(void);
int cpu_chk_interups(void);
int get_cr0(void);
void int_to_bin_str(int, char *);
void int_to_hex_str(int, char *);
void write_gdt64(void);
void write_mini_64bit_kernel(void);


void main(void){

  /* lets cause a vault */
	int FLAG = 0;
	if (FLAG == 1){
		int b = 0;
		int a = 9 / b;
	}
	int POS = 1; // keep track of lines printed
	char cpuname[16];
	unsigned int lm_bit;

	screen_clear();
	kprint("screen cleared..\n");
	print("print111");
	print("print222");
	print("arrived in 32 bit protected mode           \n");
	print("testing stack... seems to work             \n");
	char test[] = "testing data section, seems to work    \n";
	print(test);
	screen_paint(GREEN);

	// check some assumtions about the cpu
	// this assumes CPUID is avilable ont he current platform
	print("lets read CPUID..");
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

	
	print("check if interups are enabled..  \n");
	int interupts_enabled = cpu_chk_interups();
	if (interupts_enabled){
		print("interupts enbaled");
	} else {
		print("interupts DISabled");
	}

	int cr0 = get_cr0();
	char cr0_str[33];
	int_to_bin_str(cr0, cr0_str);

	print("cr0 -> ");
	print(cr0_str);


	print("starting transition to 64bit long mode     \n");
	print("setting up paging");
	set_up_paging();

	cprint("apparently we are in long mode.... AND PAGING", YELLOW);

	int iiiii  = 0;
	for(;;){
		//kprint_yx(".", POS++, 0, D);
		iiiii --;
	}
}

int get_cr0(void){
	int cr0;
	__asm__ volatile(
		"mov cr0, eax":"=a"(cr0));
	return cr0;
}


void int_to_bin_str(int n, char * str){
	for (int i = 0; i < 32; i++){
		str[i] = ((n >> (31 - i)) & 1 ) + '0';
	}
}

void int_to_hex_str(int n, char * str){
	char map[] = "0123456789abcdef";
	int mask = 0b1111;
	int i = 0;
	for (; i < 8; i++){
		int ix = ( n >> ((7 - i) * 4) ) & mask;
		str[i] = map[ix];
	}
	str[i] = '\0'; 
}

int cpu_chk_interups(void){
	unsigned int flags;
	__asm__ volatile ("pushf\n"
		"pop eax"
		:"=a"(flags));
	return flags & (1 << 9);
}

void cpu_id(const char * cpuname) {
	// get the cpu vendor given name
	//cpuid_string(0, (int*)(cpuname));

	int code = 0; /// magic number, CPUI function
	int * where = (int *) cpuname;

	__asm__ volatile (
		"cpuid"
		:"=a"(*where),"=b"(*(where+0)),"=d"(*(where+1)),"=c"(*(where+2))
		:"a"(code));
	//return (int)where[0];
	//return * where;
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

void write_gdt64(void){

	/// lets stuff the gtd64 intto 0x500

	//lgdt [gdt64.pointer]
	//jmp gdt64.code:longstart
	//
	//gdt64:
	//    dq 0
	//.code: equ $ - gdt64
	//    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
	//.pointer:
	//    dw $ - gdt64 - 1 ; length of the gdt64 table
	//    dq gdt64         ; addess of the gdt64 table
	//

	// 3x dq + dw 
	// dq -> 8 byte
	// dw -> 2 bytes
	// => 26?!?
	
	// uint64_t mask = (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53);
	uint8_t *gdt64 = (uint8_t *) 0x500;
	//gdt64[8] = mask; // WWONT ORK
	//uint64_t *gdt64_8 = (uint64_t *) 0x508;


	*gdt64++ = 0x00;
	*gdt64++ = 0x00;
	*gdt64++ = 0x00;
	*gdt64++ = 0x00;

	*gdt64++ = 0x00;
	*gdt64++ = 0x00;
	*gdt64++ = 0x00;
	*gdt64++ = 0x00;

	*gdt64++ = 0x66;
	*gdt64++ = 0x0F;
	*gdt64++ = 0x01;
	*gdt64++ = 0x0D;

	*gdt64++ = 0xFB;
	*gdt64++ = 0x02;
	*gdt64++ = 0x00;
	*gdt64++ = 0x00;

	*gdt64++ = 0x48;
	*gdt64++ = 0x8D;
	*gdt64++ = 0x1D;
	*gdt64++ = 0xF4;

	*gdt64++ = 0x02;
	*gdt64++ = 0x00;
	*gdt64++ = 0x00;
	*gdt64++ = 0x00;

	// 0x500 - 0x507 |  00 00 00 00 00 00 00 00  ; gdt64: dq 0
	// 0x508 - 0x50F |  66 0F 01 0D FB 02 00 00  ; .pointer: dw $ - gdt64 - 1 ; length of the gdt64 table
	// 0x510 - 0x517 |  48 8D 1D F4 02 00 00     ;          dq gdt64         ; address of the gdt64 table

	//gdt64[16] = 26; // words??
	//gdt64[16] = 52; // bytes!?!?
	

	/// ▷⋅⋅⋅//    dq gdt64         ; addess of the gdt64 table 
	//gdt64[17] = "0x5";
	//gdt64[18] = 0x0;
	
}

void write_mini_64bit_kernel(void){
	// 0x00007E00
	uint8_t *buffer = (uint8_t*)0x7E00;

	// 00000000: 66c7 0425 0080 0b00 4f0e 66c7 0425 0280  f..%....O.f..%..
	// 00000010: 0b00 4b0e f4   

	*buffer++ = 0x66 ;
	*buffer++ = 0xc7 ;
	*buffer++ = 0x04 ;
	*buffer++ = 0x25 ;

	*buffer++ = 0x00 ;
	*buffer++ = 0x80 ;
	*buffer++ = 0x0b ;
	*buffer++ = 0x00 ;

	*buffer++ = 0x4f ;
	*buffer++ = 0x0e ;
	*buffer++ = 0x66 ;
	*buffer++ = 0xc7 ;

	*buffer++ = 0x04 ;
	*buffer++ = 0x25 ;
	*buffer++ = 0x02 ;
	*buffer++ = 0x80 ;

	*buffer++ = 0x0b ;
	*buffer++ = 0x00 ;
	*buffer++ = 0x4b ;
	*buffer++ = 0x0e ;

	*buffer++ = 0xf4 ;
}

void set_up_paging(void){
// https://github.com/starsheriff/train-os/tree/master/06-switch-to-long-mode
	write_mini_64bit_kernel();
	write_gdt64(); // to 0x500

/*
	; Step 2: Enable Physical Address Extension
	mov eax, cr4
	or eax, (1 << 5)
	mov cr4, eax
*/
	/// tested, woks.. (creg -> CR4=0x00000020)
	__asm__ volatile (
		"mov eax, cr4 \n"
		"or eax, (1 << 5 ) \n"
		"mov cr4, eax \n");


	// this must align to 4096
	uint8_t *buffer = (uint8_t*)0x10000;

	// lets null this just to be sure
	uint32_t buffer_len = 4096 * 3;
	for (uint32_t i = 0; i < buffer_len; i ++){
		buffer[i] = 0x00;
	}

	// lets also null this
	uint32_t * gbuffer = (uint32_t *) 0x500;
	for (uint32_t i = 0; i < 100; i ++){
		gbuffer[i] = 0x00;
	}


	// out page tables. 4096 byte each..
	uint32_t *p4_table = (uint32_t*) 0x10000;
	uint32_t *p3_table = (uint32_t*) 0x11000;
	uint32_t *p2_table = (uint32_t*) 0x12000;

	/*
	*	; Step 3: Set `cr3` register
	*	mov eax, p4_table
	*	mov cr3, eax
	*/
	// -> CR3=0x000000010000
	__asm__ volatile (
		"mov eax, %0  \n"
		"mov cr3, eax \n"
		: : "r"(p4_table));


	// ; Step 4: Set the p2[1] entry to point to the _second_ 2 MiB frame
	// mov eax, (0x20_0000 | PDE_PRESENT | PDE_WRITABLE | PDE_LARGE)
	// mov [p2_table + 8], eax
	//
	// ; point the 0th entry to the first frame
	// mov eax, (0x00_0000 | PDE_PRESENT | PDE_WRITABLE | PDE_LARGE)
	// mov [p2_table], eax


	uint32_t PDE_PRESENT  = 1 << 0;
	uint32_t PDE_WRITABLE = 1 << 1;
	uint32_t PDE_LARGE    = 1 << 7;

	*p2_table = (0x000000 | PDE_PRESENT | PDE_WRITABLE | PDE_LARGE);  //    0x83

	*(p2_table + 8) = (0x200000 | PDE_PRESENT | PDE_WRITABLE | PDE_LARGE);  // 0x20183


	//; Step 5: Set the 0th entry of p3 to point to our p2 table
	//mov eax, p2_table ; load the address of the p2 table
	//or eax, (PDE_PRESENT | PDE_WRITABLE)
	//mov [p3_table], eax

	*p3_table = ((uint32_t)p2_table | PDE_PRESENT | PDE_WRITABLE);


	//; Step 6: Set the 0th entry of p4 to point to our p3 table
	//mov eax, p3_table
	//or eax, (PDE_PRESENT | PDE_WRITABLE)
	//mov [p4_table], eax

	*p4_table = ((uint32_t)p3_table | PDE_PRESENT | PDE_WRITABLE);


	// ; Step 7: Set EFER.LME to 1 to enable the long mode
	// mov ecx, 0xC0000080
	// rdmsr
	// or  eax, 1 << 8
	// wrmsr

	__asm__ volatile (
		"mov ecx, 0xC0000080 \n"
		"rdmsr \n"
		"or eax, 1 << 8 \n"
		"wrmsr \n"
		);


	// ; Step 8: enable paging
	// mov eax, cr0
	// or eax, 1 << 31
	// mov cr0, eax

	__asm__ volatile (
		"mov eax, cr0 \n"
		"or eax, 1 << 31 \n"
		"mov cr0, eax "
	);


	//lgdt [gdt64.pointer]
	__asm__ volatile("lgdt [0x510] \n");


	// now we jump mem location 0x7E00
	// is the segment 0 or 1?
	// or is it code aka 8?
	__asm__ volatile ("jmp 8:0x7E00 \n");


	/// We dont need a stack, no?
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

