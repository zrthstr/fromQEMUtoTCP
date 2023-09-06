
int kprint(const char *string);

void main(void){

	kprint("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa\n");

	char test[] = "ABCBBBBBBBBBBBb\n";
	kprint(test);
	for(;;){
	}
	return ;
}

int kprint(const char *string){

	char n = 0;
	char colour = 0x07;
	volatile char *video = (volatile char*)0xB8000;

	while (*string != 0){
		*video++ = *string++;
		*video++ = colour;
		n++;
	}
	return n;
}
