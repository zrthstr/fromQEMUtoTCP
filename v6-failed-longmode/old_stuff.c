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
