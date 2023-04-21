#include <stdint.h>
#include <stddef.h>
#include "std.h"

int POS = 1;

void init_heap(void){
	uint64_t * heap = (uint64_t *) (2 << 20); // 2MB
	uint64_t SIZE_OF_HEAP = (1 << 20) / sizeof(heap); // 1MB
	
	for (uint64_t i = 0; i < SIZE_OF_HEAP; i++){
		heap[i] = 0x00;
	}
}

struct meta {
	uint64_t magic;
	uint64_t size;
	uint64_t next;
};


void * malloc(size_t size){

  print("dddd");

  // magic is not relied uppon, its just there for debugging
  /// so we need to decide if we return the pointer to the first entry of the struct
  /// or rather return the pointer to the data part..
  /// clearly the data part is the better idea..

	uint64_t * heap = (uint64_t *) (2 << 20);
	uint64_t magic = 0b1111000011110000101010101010101011001100110011000011001100110011;

	struct meta * block;
	for(;;){
		// test if next is 0 ... 
		block = (struct meta *) heap;

		if (block->next == 0)
			{
			// this means we found the last block
			block->magic = magic;
			block->size = size;
			block->next = (uint64_t ) block + size + sizeof(struct meta);
			return block + sizeof(struct meta);
			}
		else
			{
			// not at end yet..
			
			heap = (uint64_t) heap + block->size + sizeof(struct meta);
			}
	}
}

void free(void *ptr){
	// remove the entry form the heap table
	// TBD, but who is kidding themselfs here...
	// memmory holse? so what?
	return;
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

void cpu_id(const char * cpuname) {
	// get the cpu vendor given name

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
