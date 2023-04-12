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

int get_cr0(){
	int cr0;
	__asm__ volatile(
		"mov cr0, eax":"=a"(cr0));
	return cr0;
};


void int_to_bin_str(int n, char * str){
	for (int i = 0; i < 32; i++){
		str[i] = ((n >> (31 - i)) & 1 ) + '0';
	}
};

void int_to_hex_str(int n, char * str){
	char map[] = "0123456789abcdef";
	int mask = 0b1111;
	int i = 0;
	for (; i < 8; i++){
		int ix = ( n >> ((7 - i) * 4) ) & mask;
		str[i] = map[ix];
	}
	str[i] = '\0'; 
};

int cpu_chk_interups(void){
	unsigned int flags;
	__asm__ volatile ("pushf\n"
		"pop eax"
		:"=a"(flags));
	return flags & (1 << 9);
}

cpu_id(const char * cpuname) {
	// get the cpu vendor given name
	//cpuid_string(0, (int*)(cpuname));

	int code = 0; /// magic number, CPUI function
	int * where = cpuname;

	__asm__ volatile (
		"cpuid"
		:"=a"(*where),"=b"(*(where+0)),"=d"(*(where+1)),"=c"(*(where+2))
		:"a"(code));
	return (int)where[0];
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

void set_up_paging(){
// https://github.com/starsheriff/train-os/tree/master/06-switch-to-long-mode

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


	//uint8_t * buffer = (uint8_t *) 0x10000;
	//char *buffer = (char*)0x10000;
	
	// this must align to 4096
	uint8_t *buffer = (uint8_t*)0x10000;

	/// TODO: lets null this just to be sure
	uint32_t buffer_len = 4096 * 3;
	for (int i = 0; i < buffer_len; i ++){
		buffer[i] = 0x00;
	}

	uint8_t *p4_table = (uint8_t*) 0x10000;
	uint8_t *p3_table = (uint8_t*) 0x11000;
	uint8_t *p2_table = (uint8_t*) 0x12000;

	/*
	*	; Step 3: Set `cr3` register
	*	mov eax, p4_table
	*	mov cr3, eax
	*/
	// -> CR3=0x000000010000
	int cr3 = p4_table;
	__asm__ volatile (
		"mov eax, %0  \n"
		"mov cr3, eax \n"
		: : "r"(cr3));

	//PDE_PRESENT  equ 1 << 0
	//PDE_WRITABLE equ 1 << 1
	//PDE_LARGE    equ 1 << 7

    //  mov eax, (0x20_0000 | PDE_PRESENT | PDE_WRITABLE | PDE_LARGE)
	//  mov [p2_table + 8], eax

	uint32_t PDE_PRESENT = 1 << 0;
	uint32_t PDE_WRITABLE = 1 << 1;
	uint32_t PDE_LARGE = 1 << 7;

	uint32_t v_block_0 = 0x000000;
	uint32_t v_block_1 = 0x200000;
	uint32_t p2_table_mask_1 = (v_block_1 | PDE_PRESENT | PDE_WRITABLE | PDE_LARGE);

	// 0x0000000000012000 <bogus+       0>:	0x00000000	0x00000000	0x00200083
	// -> '0b1000000000000010000011'
	__asm__ volatile (
		"mov eax, %0\n"
		"mov [%1 + 8], eax\n"
		:
		:"r"(p2_table_mask_1), "r"(p2_table)
		:"eax");

	uint32_t p2_table_mask_0 = (v_block_0 | PDE_PRESENT | PDE_WRITABLE | PDE_LARGE);
	__asm__ volatile (
		"mov eax, %0\n"
		"mov [%1], eax\n"
		:
		:"r"(p2_table_mask_0), "r"(p2_table)
		:"eax");

	//; Step 5: Set the 0th entry of p3 to point to our p2 table
	//mov eax, p2_table ; load the address of the p2 table
	//or eax, (PDE_PRESENT | PDE_WRITABLE)
	//mov [p3_table], eax

	uint32_t p3_0 = ( (uint32_t)  p2_table | PDE_PRESENT | PDE_WRITABLE);
	__asm__ volatile (
		"mov eax, %0\n"
		"mov [%1], eax\n"
		::"p"(p3_0), "p"(p3_table));


	//; Step 6: Set the 0th entry of p4 to point to our p3 table
	//mov eax, p3_table
	//or eax, (PDE_PRESENT | PDE_WRITABLE)
	//mov [p4_table], eax

	uint32_t p4_0 = ((uint32_t) p3_table | PDE_PRESENT | PDE_WRITABLE);
	__asm__ volatile (
		"mov eax, %0\n"
		"mov [%1], eax\n"
		::"p"(p4_0), "p"(p4_table));


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
	
	uint64_t mask = (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53);
	uint8_t *gdt64 = (uint8_t *) 0x500;
	//gdt64[8] = mask; // WWONT ORK
	uint64_t gdt64_8 = (uint64_t *) 0x508;
	gdt64_8 = mask;

	gdt64[16] = 26 // words??
	gdt64[16] = 52 // bytes??
	

	ONLY THIS LEFT
	/// ▷⋅⋅⋅//    dq gdt64         ; addess of the gdt64 table 
	gdt64_8[]... 

}

void set_up_paging_copy_new_finaldotexedotdoceleven(){

	
	///// https://wiki.osdev.org/Setting_Up_Long_Mode

	/* 
	 * start by nulling the memory where we want out page tables to go
	 *
	 *   mov edi, 0x1000    ; Set the destination index to 0x1000.
	 *   mov cr3, edi       ; Set control register 3 to the destination index.
	 *   xor eax, eax       ; Nullify the A-register.
	 *   mov ecx, 4096      ; Set the C-register to 4096.
	 *   rep stosd          ; Clear the memory.
	 *   mov edi, cr3       ; Set the destination index to control register 3.
	 *
	 */
	uint8_t* buffer = (uint8_t*) 0x1000;  // Pointer to buffer starting at 0x1000
	// Clear the buffer by setting each byte to zero
	for (size_t i = 0; i < 4096; i++) {
		buffer[i] = 0;
	}

	/* 
	 *  mov DWORD [edi], 0x2003      ; Set the uint32_t at the destination index to 0x2003.
	    add edi, 0x1000              ; Add 0x1000 to the destination index.
	    mov DWORD [edi], 0x3003      ; Set the uint32_t at the destination index to 0x3003.
	    add edi, 0x1000              ; Add 0x1000 to the destination index.
	    mov DWORD [edi], 0x4003      ; Set the uint32_t at the destination index to 0x4003.
	    add edi, 0x1000              ; Add 0x1000 to the destination index.
	
noticed: we used a three. This set the first 2 bit.
These bits indicate that the page is present, readable & writable
*/


	//int edi = 0x1000;
	int edi = 0x0000;
	buffer[edi] = 0x2003;
	edi += 0x1000;
	buffer[edi] = 0x3003;
	edi += 0x1000;
	buffer[edi] = 0x4003;
	edi += 0x1000;

/*  Now lets identity map the first two megabytes:
	    mov ebx, 0x00000003          ; Set the B-register to 0x00000003.
	    mov ecx, 512                 ; Set the C-register to 512.
	
	.SetEntry:
	    mov DWORD [edi], ebx         ; Set the uint32_t at the destination index to the B-register.
	    add ebx, 0x1000              ; Add 0x1000 to the B-register.
	    add edi, 8                   ; Add eight to the destination index.
	    loop .SetEntry               ; Set the next entry.
	
	 */

// Now lets identity map the first two megabytes, in c
	int ebx = 0x00000003;
	int ecx = 512;

	// is this off by one? and would this matter?
	while (ecx > 0){
	//mov DWORD [edi], ebx         ; Set the uint32_t at the destination index to the B-register.
	buffer[edi] = ebx; // ??
	ebx += 0x1000;
	edi += 8;
	ecx--;
	}

		__asm__ volatile (
		"mov eax, cr4   \n"    // ; Set the A-register to control register 4.
		"or eax, 1 << 5 \n"    // ; Set the PAE-bit, which is the 6th bit (bit 5).
		"mov cr4, eax   \n"    // ; Set control register 4 to the A-register.
	);

	__asm__ volatile (
		"mov ecx, 0xC0000080  \n"    // ; Set the C-register to 0xC0000080, which is the EFER MSR.
		"rdmsr                \n"    // ; Read from the model-specific register.
		"or eax, 1 << 8       \n"    // ; Set the LM-bit which is the 9th bit (bit 8).
		"wrmsr                \n"    // ; Write to the model-specific register.
	);

	__asm__ volatile (
		"mov eax, cr0       \n"      //  ; Set the A-register to control register 0.
		"or eax, 1 << 31    \n"      //  ; Set the PG-bit, which is the 32nd bit (bit 31).
		"mov cr0, eax       \n"      //  ; Set control register 0 to the A-register.
		);


	// now set up a nice and propper 64 bit gtd
	/*
	Access bits
	PRESENT        equ 1 << 7
	NOT_SYS        equ 1 << 4
	EXEC           equ 1 << 3
	DC             equ 1 << 2
	RW             equ 1 << 1
	ACCESSED       equ 1 << 0

	; Flags bits
	GRAN_4K       equ 1 << 7
	SZ_32         equ 1 << 6
	LONG_MODE     equ 1 << 5

	GDT:
	  .Null: equ $ - GDT
	       dq 0
	    .Code: equ $ - GDT
	        dd 0xFFFF                                   ; Limit & Base (low, bits 0-15)
	        db 0                                        ; Base (mid, bits 16-23)
	        db PRESENT | NOT_SYS | EXEC | RW            ; Access
	        db GRAN_4K | LONG_MODE | 0xF                ; Flags & Limit (high, bits 16-19)
	        db 0                                        ; Base (high, bits 24-31)
	    .Data: equ $ - GDT
	        dd 0xFFFF                                   ; Limit & Base (low, bits 0-15)
	        db 0                                        ; Base (mid, bits 16-23)
	        db PRESENT | NOT_SYS | RW                   ; Access
	        db GRAN_4K | SZ_32 | 0xF                    ; Flags & Limit (high, bits 16-19)
	        db 0                                        ; Base (high, bits 24-31)
	    .TSS: equ $ - GDT
	        dd 0x00000068
	        dd 0x00CF8900
	    .Pointer:
	        dw $ - GDT - 1
	        dq GDT
		*/


	/// GPT4: Here is a C bitwise equivalent of the given assembly code

#define PRESENT   (1 << 7)
#define NOT_SYS   (1 << 4)
#define EXEC      (1 << 3)
#define DC        (1 << 2)
#define RW        (1 << 1)
#define ACCESSED  (1 << 0)

// Flags bits
#define GRAN_4K   (1 << 7)
#define SZ_32     (1 << 6)
#define LONG_MODE (1 << 5)


struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct gdt_entry gdt[3];
struct gdt_ptr gp;

void gdt_init(void) {
    // NULL descriptor
    gdt[0].limit_low = 0;
    gdt[0].base_low = 0;
    gdt[0].base_mid = 0;
    gdt[0].access = 0;
    gdt[0].granularity = 0;
    gdt[0].base_high = 0;

    // Code descriptor
    gdt[1].limit_low = 0xFFFF;
    gdt[1].base_low = 0;
    gdt[1].base_mid = 0;
    gdt[1].access = PRESENT | NOT_SYS | EXEC | RW;
    gdt[1].granularity = GRAN_4K | LONG_MODE | 0xF;
    gdt[1].base_high = 0;

    // Data descriptor
    gdt[2].limit_low = 0xFFFF;
    gdt[2].base_low = 0;
    gdt[2].base_mid = 0;
    gdt[2].access = PRESENT | NOT_SYS | RW;
    gdt[2].granularity = GRAN_4K | SZ_32 | 0xF;
    gdt[2].base_high = 0;

    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base = (uint32_t) &gdt[0];

    // Load the GDT
    __asm__ volatile ("lgdt %0" : : "m"(gp));
    // Reload all segment registers
    //asm volatile("mov $0x10, %ax \n mov %ax, %ds \n mov %ax, %es \n mov %ax, %fs \n mov %ax, %gs \n ljmp $0x08, $next \n next:");
	}

	gdt_init();


	__asm__ volatile (
		//"lgdt [GDT64.Pointer]   \n"   //  ; Load the 64-bit global descriptor table.
		//"jmp GDT64.Code:Realm64 \n"   //  ; Set the code segment and enter 64-bit long mode.
		"jmp 0x12345 \n"   //  ; Set the code segment and enter 64-bit long mode.
			);

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

