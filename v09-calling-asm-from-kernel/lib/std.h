//#ifndef std_H
//#define std_H
extern int POS;
//#endif

void * malloc(size_t size);
void free(void *ptr);
void init_heap(void);
int kprint(const char *string);
int kprint_yx(const char *string, int y, int x, char color);
void screen_clear(void);
void screen_paint(int);
//void cpuid(void);
void cpu_id(const char *cpuname);
unsigned int cpu_lm_bit(void);
//int cpu_chk_interups_64(void);
void int_to_bin_str(int, char *);
void int_to_hex_str(int, char *);

#define BLACK 0x00
#define BLUE 0x01
#define RED 0x4
#define DARKGRAY 0x07
#define D 0x07
#define LIGHTBLUE 0x09
#define GREEN 0x0A
#define LIGHTCYAN 0x0B
#define VGA_BUFF_START 0xB8000
#define VGA_BUFF_END 0xB8FA0
#define YELLOW 0x0E

#define print(x); kprint_yx(x, POS++, 0, D);
#define cprint(x, y); kprint_yx(x, POS++, 0, y);

