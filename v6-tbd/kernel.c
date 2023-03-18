#define BLACK 0x00
#define GREEN 0x0A
#define DARKGRAY 0x07
#define VGA_BUFF_START 0xB8000
#define VGA_BUFF_END 0xB8FA0

int kprint(const char *string);
int kprint_yx(const char *string, int y, int x);
void screen_clear(void);
void screen_paint(int);

void main(void){

  /* lets cause a vault */
	int FLAG = 0;
	if (FLAG == 1){
	int b = 0;
	int a = 9 / b;
	}


	screen_clear();
	kprint("screen cleared..\n");
	kprint_yx("testing stack... seems to work             \n",1,0);
	char test[] = "testing data section, seems to work      \n";
	kprint_yx(test,2,0);
	screen_paint(GREEN);

	for(;;){
	}
}

int kprint(const char *string){
	/*
	char n = 0;
	char color = DARKGRAY;
	volatile char *video = (volatile char*)0xB8000;

	while (*string != 0){
		*video++ = *string++;
		*video++ = color;
		n++;
	}
	return n;
	*/
	int x = 0;
	int y = 0;
	int n = kprint_yx(string, y, x);
	return n;
}

int kprint_yx(const char *string, int y, int x){
	/*  x => start char; y => start line */

	int n = 0;
	char color = DARKGRAY;
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

