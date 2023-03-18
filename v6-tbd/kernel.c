#define BLACK 0
#define GREEN 10
#define VGA_BUFF_START 0xB8000
#define VGA_BUFF_END 0xB8FA0

int kprint(const char *string);
void screen_clear(void);
void screen_paint(int);

void main(void){


  /* lets cause a vault */
	
	int b = 0;
	int a = 9 / b;


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
	char n = 0;
	char color = 0x07;
	volatile char *video = (volatile char*)0xB8000;

	while (*string != 0){
		*video++ = *string++;
		*video++ = color;
		n++;
	}
	return n;
}

int kprint_yx(const char *string, int y, int x){
	/*  x => start char
	 *  y => start line
	 */

	char color = 0x07;
	int offset = (x + y * 80) * 2;
	volatile char *video = (volatile char*)0xB8000 + offset;
	while ( *string != 0){
		*video++ = *string++;
		*video++ = color;
	}
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

