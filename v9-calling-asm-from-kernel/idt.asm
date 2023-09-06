section .data:
; not sure about the alignment. To be sure, I will page align the idt for now.
align 4096

; the number of available interrupts, i.e. the number of entries in the idt
; ref section 8.2 p. 216 in AMD's programmers manual
IDT_NUM_ENTRIES equ 256

; the size of a single entry in the idt, in _long mode_, is 16 bytes.
; ref section 4.6.5 p. 79 in AMD's programmers manual
IDT_ENTRY_SIZE equ 16

; total size in bytes required for the idt
IDT_TOTAL_SIZE equ IDT_NUM_ENTRIES*IDT_ENTRY_SIZE

idt:
    resb IDT_TOTAL_SIZE
