
int kprint(const char *string);

/*int kprintln(const char *string);*/

void switch_to_protected(void);

void main(void){
	/*
	switch_to_protected();
	*/
	kprint("arrived in protected mode..\n");
	for(;;){
	}
	return ;
}

void switch_to_protected(void){
	__asm__ volatile (
		"or al, 1\n"
		"mov cr0, eax"
	);
}

int kprint(const char *string){
	/* 0xB8000 is the address of the colour video buffer
	 * the buff is made of pair of char and colour ...
	 *
	 * Set up VGA text mode
	 * */

	int n = 0;
	int colour = 0xAAAAAA;
	volatile char *video = (volatile char*)0xB8000;
	/*volatile char *video = (volatile char*)0xB0000;*/

	/*
	// moving to mbr / asm
	__asm__ volatile (
        "mov al, 0x03\n\t"
        "mov ah,0x00\n\t"
        "int 0x10\n\t"
        :
        :
        : "eax"
    );
*/

	while (*string != 0){
		*video++ = *string++;
		*video++ = colour;
		n ++;
	}
	/* 
	for(;;){
	}
	*/
	return n;
}
