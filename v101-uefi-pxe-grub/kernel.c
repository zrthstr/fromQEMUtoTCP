
void kmain() {
    char *video_memory = (char *)0xb8000; // VGA text mode video memory address
    *video_memory = 'H';
    *(video_memory + 1) = 0x07; // Light grey on black
}
